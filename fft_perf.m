% fft params
fs = 200000.0;  % set signal sample frequency
fftn = 1048576;  % set fft length
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
self_test_en = 1;
gen_fin = 921.63;
gen_phase = rand() * pi;
gen_snr = 120;
gen_vpp = 0.95;
gen_hd_db = [-135 -125 -140 -140];
gen_thd = 10 * log10(sum(10 .^ (gen_hd_db / 10)));

% test data path
test_data_store = 1;
rootpath = './testdata';

fhd_search_bin = ceil(0.5 * fhdn + 1);

if self_test_en==1
    n = (0 : 1 : fftn - 1)';
	tdata = gen_vpp / 2 * cos(2 * pi * gen_fin / fs * n + gen_phase );
	tdata = awgn(tdata, gen_snr, 'measured');
    for gen_fhdn=1 : length(gen_hd_db)
        hdn_vpp = gen_vpp * power(10, gen_hd_db(gen_fhdn) / 20);
        tdata = tdata + hdn_vpp / 2 * cos(2 * pi * gen_fin * (gen_fhdn + 1) / fs * n + gen_phase );
    end
else
    tdata = code;
end

fdatax = (0 : 1 / fftn * fs : fs / 2)';

% generate windata by params
windata = zeros(size(fftn));
for k = 0 : length(win_coeff) - 1
    windata = windata +win_coeff(k+1) * (-1) ^ k * cos(2 * pi * k * n / fftn);
end
% generate windata by windows in matlab
%windata = blackmanharris(fftn)';


tdata_win = tdata .* windata;

fdatay_c = fft(tdata_win, fftn);
fdatay_c_half = fdatay_c(1 : fftn / 2 + 1);

% new
fdatay_c_half = fdatay_c_half / fftn; 
if mod(fftn, 2)==0
    fdatay_c_half(2 : fftn / 2) = fdatay_c_half(2 : fftn / 2) .* 2;
else
    fdatay_c_half(2 : fftn / 2 + 1) = fdatay_c_half(2 : fftn / 2 + 1) .* 2;
end

fdatay_r = abs(fdatay_c_half);
fdatay_r_db = 20 * log10(fdatay_r);

fdata_fdc_lr_idx = [ 1 ; 1 + win_mainlobe ];

fdatay_r_db_max = max(fdatay_r_db(fdata_fdc_lr_idx(2) : fftn / 2 + 1));
fdatay_r_db_norm = fdatay_r_db - fdatay_r_db_max;

fdatay_r_p = fdatay_r .* fdatay_r;

% fbase search
fdata_fbase_idx = find(fdatay_r_db == fdatay_r_db_max);
fdata_fbase_f = fdatax(fdata_fbase_idx);
fdata_fbase_lr_idx = [fdata_fbase_idx - win_mainlobe; fdata_fbase_idx + win_mainlobe];

% fhd search
fdata_fhd_n = 2 : 1 : fhdn;
fdata_fhd_calc_idx = fdata_fhd_n * (fdata_fbase_idx - 1) + 1;
fdata_fhd_search_lr_idx = [ fdata_fhd_calc_idx - fhd_search_bin; fdata_fhd_calc_idx + fhd_search_bin ]';

fdata_fhd_search_max = zeros(fhdn-1,1);
fdata_fhd_search_idx = zeros(fhdn-1,1);
for i=1 : fhdn-1
    fdata_fhd_search_max(i) = max(fdatay_r_p(fdata_fhd_search_lr_idx(i, 1) : fdata_fhd_search_lr_idx(i, 2)));
    fdata_fhd_search_idx(i) = find(fdatay_r_p == fdata_fhd_search_max(i));
end
fdata_fhd_f = fdatax(fdata_fhd_search_idx);
fdata_fhd_lr_idx =  [ fdata_fhd_search_idx - win_hdlobe, fdata_fhd_search_idx + win_hdlobe ];

% power calc
fdata_pdc = sum(fdatay_r_p(fdata_fdc_lr_idx(1) : fdata_fdc_lr_idx(2)));
fdata_pbase = sum(fdatay_r_p(fdata_fbase_lr_idx(1) : fdata_fbase_lr_idx(2)));
fdata_phd = zeros(fhdn-1,1);
for i=1 : fhdn-1
    fdata_phd(i) = sum(fdatay_r_p(fdata_fhd_lr_idx(i, 1) : fdata_fhd_lr_idx(i, 2)));
end
fdata_pnoise = sum(fdatay_r_p) - fdata_pdc - fdata_pbase - sum(fdata_phd);

% perf calc
snr = 10 * log10(fdata_pbase / fdata_pnoise);
thd = 10 * log10(sum(fdata_phd) / fdata_pbase);

base_db = 10 * log10(fdata_pbase);
hdn_db = 10 * log10(fdata_phd);

% print report
fprintf('%-16s %-16.2f', 'F (Hz)', fdata_fbase_f);
for i=1 : fhdn-1
    fprintf('%-16.2f', fdata_fhd_f(i));
end
fprintf('\n');
fprintf('%-16s %-16.2f', 'P (dB)', base_db);
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
    writematrix(fdatay_r_db, fullfile(storepath,'fdatay_r_db.txt'));
    writematrix(fdatay_r_db_norm, fullfile(storepath,'fdatay_r_db_norm.txt'));
    writematrix(fdatay_r_p, fullfile(storepath,'fdatay_r_p.txt'));
    
    writematrix(snr, fullfile(storepath,'snr.txt'));
    writematrix(thd, fullfile(storepath,'thd.txt'));
    writematrix(hdn_db, fullfile(storepath,'hdn_db.txt'));
end