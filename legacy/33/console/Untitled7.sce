ff = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.firDPhase\console\fft", -1, 3);

clf;

v = ff(:,2) + %i*ff(:,3);
plot(ff(:,1), imag(v),'k');
plot(ff(:,1), real(v),'b');
//plot(ff(:,1), cos(atan(imag(v), real(v))), 'b');

