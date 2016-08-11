
stacksize('max');

clf;
aap_0 = read("P:\finance\spectrumSeparator\nhel.Composite\12\console\aap_0",-1,3);
tsa = read("P:\finance\spectrumSeparator\nhel.Composite\13\console\sverta",-1,1);


orig = aap_0(:,1) .*cos(aap_0(:,3)) + %i*aap_0(:,2) .*sin(aap_0(:,3));
plot(cos(atan(imag(orig), real(orig))));
//plot(tsa/140,'r');

abort;

