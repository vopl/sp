stacksize('max');
tap = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.2\console\tap", -1, 3);
//tap1 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.2\console\tap_25", -1, 3);
//tap3 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.2\console\tap_2", -1, 3);


tt = tap(:,2).*cos(tap(:,3)) + %i*tap(:,2).*sin(tap(:,3));
//tt1 = tap1(:,2).*cos(tap1(:,3)) + %i*tap1(:,2).*sin(tap1(:,3));
//tt3 = tap3(:,2).*cos(tap3(:,3)) + %i*tap3(:,2).*sin(tap3(:,3));

clf;

//tt1 = tt1;
//plot(tap1(:,1), abs(tt1), 'g');
//plot(tap1(:,1), real(tt1), 'g');
//plot(tap1(:,1), imag(tt1), 'g');
//plot(tap1(:,1), (atan(imag(tt1), real(tt1))), 'k');

//tap(:,1) = tap(:,1)/60/60;
tt = tt;
plot(tap(:,1), abs(tt), 'r');
plot(tap(:,1), real(tt), 'b');
plot(tap(:,1), imag(tt), 'k');
//plot(tap(:,1), (atan(imag(tt), real(tt))), 'k');
abort
sc = sin((tap(:,1)-1)*20*6.3) .* (tap(:,1)-1)*0.5 + (tap(:,1)-1)/4;
plot(tap(:,1), sc, 'b');

pp = (atan(imag(tt), real(tt)));
plot(tap(:,1), pp, 'k');

