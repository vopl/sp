stacksize('max');
folder = "P:\finance\spectrumSeparator\nhel.Composite\8_tests\console\";
w = 100;

//am = read(folder+"outa", -1, w);
//pm = read(folder+"outp", -1, w);

h = size(am,1);

x=1:w;
y=1:h;

clf;
f = gcf();
f.color_map = jetcolormap(512);


//grayplot(y,x,    cos(pm)  );
//grayplot(y,x,    am  );
//plot(    (am(:,51))     );
//plot(    (am(1000,:))     );
//plot(    (pm(1000,:))     );

for i=1:10:10 do
  plot(    (am(:,i))     ,'k');
  plot(    (pm(:,i))     ,'b');
end;
//  plot(    (am(:,1))     ,'k');
//  plot(    cos(2*%pi*(y-3051.5)/ ((6606-3051.5)/10) )     ,'b');

