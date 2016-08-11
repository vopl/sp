stacksize('max');

w = 682;
//t = read("tail_t", -1, w);
a = read("tail", -1, w);

clf;
pos = size(a,1);
pos=37;
plot(a(pos, :),'r');

pos = 1;
plot(a(pos, :));

