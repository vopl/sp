stacksize('max');

clf
tap1 = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.onlyDsp\console\tap_xx", -1, 3);
tap1(:,2) = tap1(:,2).*(cos(tap1(:,3)) + %i*sin(tap1(:,3)));


tap = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.onlyDsp\console\tap", -1, 3);
tap(:,2) = tap(:,2).*(cos(tap(:,3)) + %i*sin(tap(:,3)));


plot(tap1(:,1), real(tap1(:,2)), 'b');
plot(tap(:,1), real(tap(:,2)), 'r');

//plot(tap1(:,1), tap1(:,2)+tap2(:,2)-tap(:,2), 'm');

