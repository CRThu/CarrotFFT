function [data] = adcperf_datafeed(path, filetype, wordtype, actualtype, align, sep, signcoding)
%data read from file
%   filetype: ( ascii-dec | ascii-hex | binary )
%   wordtype: ( u | i ) (8 | 16 | 24 | 32) [-] [ big | little ]
%   actualtype: ( u | i ) (0-wordtype.bits)
%   align: ( left | right )
%   sep: ( 'none' | '\n' | '\s' | ',' | ';' )
%   signcoding: ('offset' | 'comp')

% example: data2=adcperf_datafeed('./test/ascii-hex.txt','ascii-hex','u16');
% example: data2=adcperf_datafeed('./test/ascii-hex.txt','ascii-hex','u16-big','u12','left');
% example: code = datafeed(filename, 'ascii-hex', 'u32-big', 'u18', 'left');

% 参数处理与默认值
if nargin < 2 || isempty(filetype)
    filetype = 'ascii-hex';
end
if nargin < 3 || isempty(wordtype)
    wordtype = 'u16-little'; % 默认: 16位无符号小端
end
if nargin < 4 || isempty(actualtype)
    actualtype = wordtype; % 默认: wordtype
end
if nargin < 5 || isempty(align)
    align = 'left'; % 默认: 左对齐 0xAA->0xAA00
end
if nargin < 6 || isempty(sep)
    sep = {' ', '\t', '\r', '\n'}; % ASCII word分隔
end
if nargin < 7 || isempty(signcoding)
    signcoding ='comp'; % 默认: 二进制补码
end

fprintf('[datafeed]: read file path: %s\n', path);

% 解析wordtype
[~, wordtype_bits, wordtype_endian] = parse_wordtype(wordtype);
[actualtype_sign, actualtype_bits, ~] = parse_wordtype(actualtype);

% 处理
switch lower(filetype)
    case 'ascii-dec'
        error('不支持的类型：%s', filetype);
        data = process_ascii_dec(path);
    case 'ascii-hex'
        data = process_ascii_hex(path, wordtype_endian, wordtype_bits, actualtype_bits, align, sep);
    case 'binary'
        error('不支持的类型：%s', filetype);
        data = process_binary(path);
    otherwise
        error('不支持的类型：%s', filetype);
end

data = process_sign(data, actualtype_sign, actualtype_bits, signcoding);

end

%% 解析wordtype
% example: 
% i16 u16 u16b u16-big
function [sign, bits, endian] = parse_wordtype(wordtype)
tokens = regexp(wordtype, '([ui])(\d+)-?([bl]|big|little)?', 'tokens');
if isempty(tokens)
    error('Invalid wordtype format. Expected format like "u16 | u16b | u16-big"');
end
sign = tokens{1}{1};
bits = str2double(tokens{1}{2});
endian = tokens{1}{3};

if strcmpi(endian, 'b')
    endian = 'big';
elseif strcmpi(endian, 'l')
    endian = 'little';
else
    endian = 'little';
end

fprintf('[parse_wordtype]: %s,%d,%s\n', sign, bits, endian);
end

%% 子函数：处理ASCII HEX文件
% path: 文件路径
% endian: little | big
% wordbits: bytes * 8
% actualbits: 0 - wordbits
% align: left | right
% charignore: {' ', '\r', '\n'} | ' \r\n'
function data = process_ascii_hex(path, endian, wordbits, actualbits, align, charignore)
% 读取文件内容
content = fileread(path);

% 替换忽略的字符
if ~isempty(charignore)
    if iscell(charignore)
        chars = [charignore{:}];            % 合并cell为字符数组
    else
        chars = charignore;                 % 直接使用字符数组
    end
    % 构建正则表达式模式（自动处理特殊字符）
    pattern = ['[' chars ']'];
    content = regexprep(content, pattern, '');
end

% 检查内容长度是否有效
if isempty(content)
    error('文件内容为空');
end

% 分割数据项
wordbytes = ceil(wordbits / 8) * 2;
if mod(length(content), wordbytes) ~= 0
    error('文件内容长度不是wordbytes的整数倍');
end
content_reshape = reshape(content, wordbytes, [])';

% 处理大小端
bytes_per_word = wordbytes / 2;
hex_3d = permute(reshape(content_reshape', 2, bytes_per_word, []), [3 2 1]); % 三维数组
if strcmpi(endian, 'little')
    hex_3d = hex_3d(:, end:-1:1, :);        % 反转字节顺序
end
content_reshape = reshape(permute(hex_3d, [1 3 2]), [], wordbytes);

% 转换为十进制数 (hex2dec only support 53bit)
data_dec = hex2dec(content_reshape);

% 处理位对齐
if actualbits > wordbits
    error('actualbits不能超过wordbits');
end
if actualbits < wordbits
    switch lower(align)
        case 'left'
            data_dec = bitshift(data_dec, actualbits - wordbits);  % 左移去高位
        case 'right'
            data_dec = bitand(data_dec, 2^actualbits - 1);         % 掩码保留低位
        otherwise
            error('不支持的对齐方式：%s', align);
    end
end

data = uint64(data_dec);
end

%% 子函数：处理ASCII DEC文件
function [data] = process_ascii_dec(path)

end

%% 子函数：处理二进制文件
function data = process_binary(path)
end


%% 子函数：处理符号
% data: 原始数据
% sign: 'u' | 'i'
% bits: uint, 0-64
% signcoding: 'offset' | 'comp'
function out = process_sign(data, sign, bits, signcoding)
    switch lower(sign)
        case 'u'
            out = data;
        case 'i'
            u64mask = typecast(int64(-1),'uint64');
            
            switch lower(signcoding)
                case 'offset'
                    out = int64(data) - bitset(int64(0), bits);
                case 'comp'
                    sign = bitget(uint64(data), bits);
                    mask = bitshift(u64mask, bits, 'uint64');
                    out = bitor(uint64(data), sign * mask);
                    out = typecast(out,'int64');
                otherwise
                    error('不支持的符号编码：%s', signcoding);
            end
        otherwise
            error('不支持的符号位：%s', sign);
    end
end