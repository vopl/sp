stacksize('max');


echo = read("echo", -1,3);
echot = echo(:,1);
echov = echo(:,2)+%i*echo(:,3);

model = read("model", -1,3);
modelt = model(:,1);
modelv = model(:,2)+%i*model(:,3);

clf;
plot(abs(echov));
plot(abs(modelv),'r');

