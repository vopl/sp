stacksize('max');
N=1860;
A = read("A", N, N);

clf;
f = gcf();
f.color_map = jetcolormap(512);

x = 1:N;
//grayplot(x,x,abs(A));

plot(A(1,:));
