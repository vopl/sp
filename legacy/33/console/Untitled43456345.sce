
tt = read("P:\finance\spectrumSeparator\nhel.Composite\20.1.win.3\console\4", -1, 5);

clf;
//plot(tt(:,1));

//plot((tt(:,2)+tt(:,4))/2,'k');
//plot(-(tt(:,2)+tt(:,4))/2 + tt(:,4),'b');
//plot(tt(:,2),'k');
//plot(tt(:,4),'k');

//plot((tt(:,3)+tt(:,5))/2,'k');
//plot(-(tt(:,3)+tt(:,5))/2 + tt(:,5),'b');
//plot(tt(:,3),'k');
//plot(tt(:,5),'k');

imm = (tt(:,3)+tt(:,5))/2;
rem = (tt(:,2)+tt(:,4))/2;

m = rem + %i*imm;

plot(abs(m));
plot(real(m));
plot(imag(m));
//plot((atan(imm, rem)));

