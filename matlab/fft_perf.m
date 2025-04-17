% datafeed
%filename = './test/nb2213_89d84.txt';
%code = adcperf_datafeed(filename, 'ascii-hex', 'u32-big', 'u18', 'left');

filename = './current/sin.txt';
portread("COM9", 2000000, filename);
code = adcperf_datafeed(filename, 'ascii-hex', 'i16-little');

% fft params
fs = 1;  % set signal sample frequency
fhdn = 5;           % set max distortion

% window params
% blackmanharris
%win_coeff = [0.35875, 0.48829, 0.14128, 0.01168];
%win_mainlobe = 4;
%win_hdlobe = 3;

% hft144d
win_coeff = [1 1.96760033 1.57983607 0.81123644 0.22583558 0.02773848 0.00090360];
win_mainlobe = 7 ;
win_hdlobe = 5;

% old program used params
%win_mainlobe = 25;
%win_hdlobe = 1;

% test data generate params
self_test_en = 0;
gen_fin = 921.63;
gen_fftn = 1048576;
gen_phase = rand() * pi;
gen_snr = 160;
gen_vpp = 0.95;
gen_hd_db = [-200 -200 -200 -200];
gen_thd = 10 * log10(sum(10 .^ (gen_hd_db / 10)));

% test data path
test_data_store = 1;
rootpath = './testdata';

fhd_search_bin = 5;
%fhd_search_bin = ceil(0.5 * fhdn + 1);


if self_test_en==1
    fftn = gen_fftn;
	tdata = gen_vpp / 2 * cos(2 * pi * gen_fin / fs * n + gen_phase );
	tdata = awgn(tdata, gen_snr, 'measured');
    for gen_fhdn=1 : length(gen_hd_db)
        hdn_vpp = gen_vpp * power(10, gen_hd_db(gen_fhdn) / 20);
        tdata = tdata + hdn_vpp / 2 * cos(2 * pi * gen_fin * (gen_fhdn + 1) / fs * n + gen_phase );
    end
else
    fftn = length(code);
    tdata = code;
end

n = (0 : 1 : fftn - 1);
fftn_half = floor(fftn / 2) + 1;
fdatax = (0 : 1 / fftn * fs : fs / 2)';

% generate windata by params
windata = zeros(size(fftn));
k = (0 : 1 : length(win_coeff) - 1)';
windata = sum(((-1) .^ k) .* win_coeff(k+1)' .* cos(k * (2 * pi * n / fftn)));
% generate windata by windows in matlab
%windata = blackmanharris(fftn)';


tdata = double(tdata);
tdata_win = tdata .* windata';

fdatay_c = fft(tdata_win, fftn);
fdatay_c_half = fdatay_c(1 : fftn_half);
fdatay_r = abs(fdatay_c_half);

% norm
fdatay_r = fdatay_r / fftn; 
fdatay_r(2 : ceil(fftn / 2)) = fdatay_r(2 : ceil(fftn / 2)) .* 2;

% fsignal search
fdata_fdc_lr_idx = [ 1 ; 1 + win_mainlobe ];
fdatay_r_max = max(fdatay_r(fdata_fdc_lr_idx(2) : fftn_half));
fdata_fsignal_idx = find(fdatay_r == fdatay_r_max);
fdata_fsignal_f = fdatax(fdata_fsignal_idx);
fdata_fsignal_lr_idx = [fdata_fsignal_idx - win_mainlobe; fdata_fsignal_idx + win_mainlobe];

% fhd search
fdata_fhd_n = 2 : 1 : fhdn;
fdata_fhd_idx_est = fdata_fhd_n * (fdata_fsignal_idx - 1) + 1;
fdata_fhd_search_lr_idx = [ fdata_fhd_idx_est - fhd_search_bin; fdata_fhd_idx_est + fhd_search_bin ]';

fdata_fhd_search_max = zeros(fhdn-1,1);
fdata_fhd_search_idx = zeros(fhdn-1,1);
for i=1 : fhdn-1
    fdata_fhd_search_max(i) = max(fdatay_r(fdata_fhd_search_lr_idx(i, 1) : fdata_fhd_search_lr_idx(i, 2)));
    fdata_fhd_search_idx(i) = find(fdatay_r == fdata_fhd_search_max(i));
end
fdata_fhd_f = fdatax(fdata_fhd_search_idx);
fdata_fhd_lr_idx =  [ fdata_fhd_search_idx - win_hdlobe, fdata_fhd_search_idx + win_hdlobe ];

% power calc
fdatay_r_p = fdatay_r .* fdatay_r;
p_dc = sum(fdatay_r_p(fdata_fdc_lr_idx(1) : fdata_fdc_lr_idx(2)));
p_signal = sum(fdatay_r_p(fdata_fsignal_lr_idx(1) : fdata_fsignal_lr_idx(2)));
p_hd = zeros(fhdn-1,1);
for i=1 : fhdn-1
    p_hd(i) = sum(fdatay_r_p(fdata_fhd_lr_idx(i, 1) : fdata_fhd_lr_idx(i, 2)));
end
p_noise = sum(fdatay_r_p) - p_dc - p_signal - sum(p_hd);

% perf calc
snr = 10 * log10(p_signal / p_noise);
thd = 10 * log10(sum(p_hd) / p_signal);

signal_db = 10 * log10(p_signal);
hdn_db = 10 * log10(p_hd);

% db data chart (norm to 0dB)
fdatay_r_p_ref = fdatay_r_p(fdata_fsignal_idx);
fdatay_r_p_norm = fdatay_r_p / fdatay_r_p_ref;
fdatay_r_p_norm_db = 10 * log10(fdatay_r_p / fdatay_r_p_ref);

% print report
fprintf('%-16s %-16.2f', 'F (Hz)', fdata_fsignal_f);
for i=1 : fhdn-1
    fprintf('%-16.2f', fdata_fhd_f(i));
end
fprintf('\n');
fprintf('%-16s %-16.2f', 'P (dB)', signal_db);
for i=1 : fhdn-1
    fprintf('%-16.2f', hdn_db(i));
end
fprintf('\n');

fprintf('%-16s %-16s\n', 'SNR (dB)', 'THD (dB)');
fprintf('%-16.2f %-16.2f\n', snr, thd);

% test data gen
if test_data_store == 1
    if self_test_en == 1
        data_name = sprintf('fft@%dpt,%.0fdb,%.0fdb', fftn, gen_snr, gen_thd);
    else
        data_name = sprintf('fft@%dpt,%.0fdb,%.0fdb', fftn, snr, thd);
    end
    
    storepath = fullfile('.', rootpath, data_name);
    
    if ~isfolder(storepath)
        mkdir(storepath);
    end
    
    writematrix(tdata, fullfile(storepath,'tdata.txt'));
    writematrix(fdatax, fullfile(storepath,'fdatax.txt'));
    writematrix(windata, fullfile(storepath,'windata.txt'));
    writematrix(tdata_win, fullfile(storepath,'tdata_win.txt'));
    writematrix(fdatay_c, fullfile(storepath,'fdatay_c.txt'));
    writematrix(fdatay_r_p, fullfile(storepath,'fdatay_r_p.txt'));
    writematrix(fdatay_r_p_norm_db, fullfile(storepath,'fdatay_r_p_norm_db.txt'));
    
    writematrix(snr, fullfile(storepath,'snr.txt'));
    writematrix(thd, fullfile(storepath,'thd.txt'));
end