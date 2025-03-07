% fft params
fs = 800000.0;  % set signal sample frequency
fftn = 131072;  % set fft length
fhdn = 5;           % set max distortion

% window params
% blackmanharris
%win_coeff = [0.35875, 0.48829, 0.14128, 0.01168];
%win_mainlobe = 4;
%win_hdlobe = 3;

% hft144d
win_coeff = [1 1.96760033 1.57983607 0.81123644 0.22583558 0.02773848 0.00090360];
win_mainlobe =7 ;
win_hdlobe = 5;

% old program used params
%win_mainlobe = 25;
%win_hdlobe = 1;

% fake data generate params
self_test_en = 1;
gen_fin = 921.63;
gen_phase = rand() * pi;
gen_snr = 120;

fhd_search_bin = ceil(0.5 * fhdn + 1);

if self_test_en==1
    n = 0 : 1 : fftn - 1;
	tdata = 0.5 * cos(2 * pi * gen_fin / fs * n + gen_phase );
	tdata = awgn(tdata, gen_snr, 'measured');
else
    tdata = code';
end

fdatax = 0 : 1 / fftn * fs : fs / 2;

% generate windata by params
windata = zeros(size(fftn));
for k = 0 : length(win_coeff) - 1
    windata = windata +win_coeff(k+1) * (-1) ^ k * cos(2 * pi * k * n / fftn);
end
% generate windata by windows in matlab
%windata = blackmanharris(fftn)';


tdata_win = tdata .* windata;

fdatay_c = fft(tdata_win, fftn);
%fdatay_c = fdatay_c / fftn;
fdatay_c_half = fdatay_c(1 : fftn / 2 + 1);
%fdatay_c_half(2 : fftn / 2 + 1) = fdatay_c_half(2 : fftn / 2 + 1) .* 2;

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