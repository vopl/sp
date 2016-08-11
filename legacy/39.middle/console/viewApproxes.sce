stacksize('max');

clf; 
aap_0 = read("P:\finance\spectrumSeparator\nhel.Composite\12\console\aap_0",-1,3);
aap_1 = read("P:\finance\spectrumSeparator\nhel.Composite\12\console\aap_1",-1,3);
//hil = read("P:\finance\spectrumSeparator\nhel.Composite\12\console\hilbert",-1,2);
//poly1 = read("P:\finance\spectrumSeparator\nhel.Composite\12\console\poly1",-1,3);
//poly2 = read("P:\finance\spectrumSeparator\nhel.Composite\12\console\poly2",-1,3);


//plot(aap_1(:,1)-aap_1(:,2));
plot(aap_1(:,1));
plot(aap_1(:,2));
//plot(-aap_1(:,1),'r');
abort;

//plot(aap_1(:,1), 'r');
//plot(aap_1(:,2));
//plot(aap_1(:,3),'r');

//plot(hil(:, 1));
//plot(cos(hil(:, 2)),'k');


//plot(cosPoly(:, 1), cosPoly(:, 2)-cosPoly(:, 5));
//plot(cosPoly(:, 1), cosPoly(:, 2),'k');
//plot(cosPoly(:, 1), cosPoly(:, 5),'r');
//abort


plot(pcat(:, 1), (aap_0(:,1)));
//plot(pcat(:, 1), pcat(:, 2),'m');
plot(cosPoly(:, 1), cosPoly(:, 5),'r');
plot(cosPoly(:, 1), cosPoly(:, 3),'m');
plot(cosPoly(:, 1), cosPoly(:, 4),'m');

//plot(pcat(:, 1), -pcat(:, 2),'m');
//plot(pcat(:, 1), pcat(:, 3),'k');
//wnd = 3000:10000;
//plot(pcat(wnd, 1), pcat(wnd, 2) .* cos(2*%pi*pcat(wnd, 1)./pcat(wnd, 3)));
//plot(pcat(wnd, 1), cos(2*%pi*pcat(wnd, 1)./pcat(wnd, 3)));



