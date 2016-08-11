stacksize('max');
tap0 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap0", -1, 3);
tap1 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap1", -1, 3);
tap = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.4note\console\tap", -1, 3);


clf;
tt0 = tap0(:,2).*cos(tap0(:,3)) + %i*tap0(:,2).*sin(tap0(:,3));
tt1 = tap1(:,2).*cos(tap1(:,3)) + %i*tap1(:,2).*sin(tap1(:,3));
tt = tap(:,2).*cos(tap(:,3)) + %i*tap(:,2).*sin(tap(:,3));

//plot(tap1(:,1), imag(tt1), 'm');
//plot(tap1(:,1), real(tt1), 'r');

//plot(tap(:,1), imag(tt), 'k');
//plot(tap(:,1), real(tt), 'b');

plot(tap(:,1), imag(tt0+tt1-tt), 'k');
plot(tap(:,1), real(tt0+tt1-tt), 'b');

//plot(tap1(:,1), imag(tt1)-imag(tt), 'k');
//plot(tap1(:,1), real(tt1)-real(tt), 'b');


