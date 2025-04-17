classdef adcperf
    %adcperf_report 此处显示有关此类的摘要
    %   此处显示详细说明
    
    properties
        % adcperf config
        fftn % fft数据截取长度
        fsample % 采样频率
        fullscale % 满幅电压
        fhdn % 最大谐波次数
        winname % 窗函数选择
        
        % fft report
     	tdata % 时域数据, 时间
        ydata % 时域数据, 电压
        fdata % 频域数据, 频率
        pdata % 频域数据, 功率
        pdata_db % 频域数据, dB
        pdata_norm0db % 频域数据, dB, 归一化到0dB
        
        % adc perf report
        fsignal
        fdistortion
        fspur
        
        psignal
        pdistortion
        pspur
        
        psignal_dbnorm
        pdistortion_dbnorm
        pspur_dbnorm
        
        snr
        sinad
        thd
        sfdr
        enob
        
        snr_0db
        sinad_0db
        thd_0db
        sfdr_0db
        enob_0db
    end
    
    methods
        function obj = adcperf(varargin)
            %adcperf 性能计算
            %   ydata(必需参数): array, 时域数据
            %   fftn(可选参数, 默认: 0): int, fft数据截取长度, 0: 全部长度
            %   fsample(可选参数, 默认: 1): double, 采样频率
            %   fullscale(可选参数, 默认: 0): double, 满幅电压, 0: 输入信号即满幅
            %   fhdn(可选参数, 默认: 5): int, 最大谐波次数
            %   winname(可选参数, 默认: hft144d): string, 窗函数选择, (blackmanharris | hft144d)
            
            % 传入参数解析
            p = inputParser;
            
            addRequired(p, 'ydata');
         	addOptional(p,'fftn', 0);
         	addOptional(p,'fsample', 1);
        	addOptional(p,'fullscale', 0);
            addOptional(p,'fhdn', 5);
          	addOptional(p,'winname', 'hft144d');

         	parse(p,varargin{:});

        	params = p.Results;
         	props = fieldnames(params);
            for i = 1:length(props)
                obj.(props{i}) = params.(props{i});
            end
            
            % 默认数据处理
            if obj.fftn == 0
                obj.fftn = length(code);
            end
            
            if obj.fullscale == 0
                obj.fullscale = max(code)-min(code);
            end
            
            % 属性生成
            n = (0 : 1 : fftn - 1)';
            fftn_half = floor(fftn / 2) + 1;
            obj.fdata = (0 : 1 / fftn * fs : fs / 2)';
            

        end
    end
end

