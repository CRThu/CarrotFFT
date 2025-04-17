% 串口异步读取数据并保存到文件的 Demo
function portread(portName, baudRate, outputFile)
    % 串口配置
    %portName = "COM9";      % 修改为你的串口号
    %baudRate = 2000000;        % 修改为你的波特率
    bytesToRead = 1;       % 每次触发回调的字节数
    %outputFile = './current/sin.txt'; % 输出文件名
    
    % 创建串口对象
    if ~isempty(instrfind)
        fclose(instrfind);
        delete(instrfind);
    end
    
    try
        port = serialport(portName, baudRate);
        
        % 打开输出文件（追加模式）
        fileID = fopen(outputFile, 'w');
        if fileID == -1
            error('无法打开文件 %s', outputFile);
        end
        
        % 配置回调函数
        configureCallback(port, "byte", bytesToRead, ...
            @(src, ~) readAndSaveData(src, fileID));
        
        fprintf('串口监听已启动，数据将保存到 %s\n', outputFile);
        fprintf('按任意键停止监听...\n');
        pause; % 等待用户按键停止
        
        % 清理工作
        configureCallback(port, "off");
        fclose(fileID);
        clear port;
        fprintf('监听已停止，数据已保存。\n');
        
    catch ME
        % 错误处理
        fprintf('发生错误: %s\n', ME.message);
        if exist('fileID', 'var') && fileID ~= -1
            fclose(fileID);
        end
        if exist('port', 'var')
            clear port;
        end
    end
end

% 回调函数：读取数据并保存到文件
function readAndSaveData(src, fileID)
    if src.NumBytesAvailable == 0
        return;
    end
    
    % 读取可用数据
    data = read(src, src.NumBytesAvailable, "uint8");
    
    % 将数据转换为十六进制字符串（如 "AA BB CC"）
    hexStr = strjoin(string(dec2hex(data, 2)), ' ');
    
    % 添加时间戳
    logLine = sprintf('%s ', hexStr);
    
    % 写入文件
    fprintf(fileID, logLine);
    
    % 同时在命令行显示（可选）
    fprintf('Received %d bytes data.\n', length(data));
end