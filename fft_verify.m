fs = 100000.0;
fin = 921.63;
fftn = 131072;

p = rand() * pi;
gen_snr = 100;

% blackmanharris
%win_coeff = [0.35875, 0.48829, 0.14128, 0.01168];
%win_mainlobe = 4;
% hft144d
win_coeff = [1 1.96760033 1.57983607 0.81123644 0.22583558 0.02773848 0.00090360];
win_mainlobe =7 ;


n = 0 : 1 : fftn - 1;
tdata = 0.5 * cos(2 * pi * fin / fs * n + p );
tdata = awgn(tdata, gen_snr, 0);
tdata = code';

fdatax = 0 : 1 / fftn * fs : fs / 2;

windata = zeros(size(fftn));
for k = 0 : length(win_coeff) - 1
    windata = windata +win_coeff(k+1) * (-1) ^ k * cos(2 * pi * k * n / fftn);
end

tdata_win = tdata .* windata;

fdatay_c = fft(tdata_win, fftn);
%fdatay_c = fdatay_c / fftn;
fdatay_c_half = fdatay_c(1 : fftn / 2 + 1);
%fdatay_c_half(2:fftn / 2 + 1)=fdatay_c_half(2:fftn / 2 + 1) .* 2;

fdatay_r = abs(fdatay_c_half);
fdatay_r_db = 20 * log10(fdatay_r);

fdatay_r_db_max = max(fdatay_r_db(win_mainlobe + 1 : fftn / 2 + 1));
fdatay_r_db_norm = fdatay_r_db - fdatay_r_db_max;

fdata_fbase_idx = find(fdatay_r_db == fdatay_r_db_max);
fdata_fbase_f = fdatax(fdata_fbase_idx);
fdata_fbase_l_idx = fdata_fbase_idx - win_mainlobe;
fdata_fbase_r_idx = fdata_fbase_idx + win_mainlobe;

% todo idx

fdatay_r_p = fdatay_r .* fdatay_r;
fdata_pbase = sum(fdatay_r_p(fdata_fbase_l_idx : fdata_fbase_r_idx));
fdata_pnoise = sum(fdatay_r_p) - fdata_pbase;

snr = 10 * log10(fdata_pbase / fdata_pnoise);