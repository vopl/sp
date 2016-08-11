clf;
dd = read("P:\finance\spectrumSeparator\nhel.Composite\13\console\total", -1,3);
a = read("P:\finance\spectrumSeparator\nhel.Composite\12_hilbertDetect\console\aap_0", -1, 3);

plot(dd(:,1), dd(:,2));
plot(dd(:,1)/2, a(:,2),'r');
plot(dd(:,1)/2, a(:,1),'r');
//plot(a(:,2)-dd(:,2),'r');
  
