stacksize('max');
tap0 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap_0", -1, 3);
tap1 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap_05", -1, 3);
tap2 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap_10", -1, 3);
tap3 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap_15", -1, 3);
tap4 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap_20", -1, 3);
tap5 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap_25", -1, 3);


clf;
tt0 = tap0(:,2).*cos(tap0(:,3)) + %i*tap0(:,2).*sin(tap0(:,3));
tt1 = tap1(:,2).*cos(tap1(:,3)) + %i*tap1(:,2).*sin(tap1(:,3));
tt2 = tap2(:,2).*cos(tap2(:,3)) + %i*tap2(:,2).*sin(tap2(:,3));
tt3 = tap3(:,2).*cos(tap3(:,3)) + %i*tap3(:,2).*sin(tap3(:,3));
tt4 = tap4(:,2).*cos(tap4(:,3)) + %i*tap4(:,2).*sin(tap4(:,3));
tt5 = tap5(:,2).*cos(tap5(:,3)) + %i*tap5(:,2).*sin(tap5(:,3));
tt = tap(:,2).*cos(tap(:,3)) + %i*tap(:,2).*sin(tap(:,3));

//plot(tap0(:,1), imag(tt0), 'k');
//plot(tap0(:,1), real(tt0), 'k');

//plot(tap1(:,1), imag(tt1), 'm');
plot(tap1(:,1), real(tt1), 'k');

//plot(tap2(:,1), imag(tt2), 'm');
//plot(tap2(:,1), real(tt2), 'm');

//plot(tap3(:,1), imag(tt3), 'm');
plot(tap3(:,1), real(tt3), 'b');
//plot(tap4(:,1), real(tt4), 'r');
plot(tap5(:,1), real(tt5), 'r');

//plot(tap(:,1), imag(tt), 'm');
//plot(tap(:,1), real(tt), 'r');


