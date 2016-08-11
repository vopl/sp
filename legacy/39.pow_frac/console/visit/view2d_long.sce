stacksize('max');
t = read("t", -1, 1);
tamount = size(t,1);
t = t(1:tamount);
tamount = size(t,1);

x = read("x", -1, 1);
xamount = size(x,1);

m=read("m", xamount, tamount);
v=read("v", xamount, tamount);


xx = 1:xamount;
tt = 1:tamount;
xx = ((x-x(1)) ./ (60*60));
tt = (t ./ (60*60));



clf;

f = gcf();
f.color_map = jetcolormap(512);
a = get("current_axes");

grayplot((xx), (tt), (m ./ v.^1.0) );
//grayplot((xx), (tt), (m) );
//a.log_flags='nl';
abort;

plot((tt),    m(15,:) ./ (v(15,:).^1.2),'k');
//plot(tt,    m(200,:)*300000,'m');

