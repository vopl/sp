stacksize('max');
t = read("t", -1, 1);
tamount = size(t,1);
t = t(1:tamount);
tamount = size(t,1);

x = read("x", -1, 1);
xamount = size(x,1);

m=read("e", xamount, tamount);
m2=read("e2", xamount, tamount);


xx = 1:xamount;
tt = 1:tamount;
xx = ((x-x(1)) ./ (60*60*24*5*52));
tt = (t ./ (60*60*24));



clf;

f = gcf();
f.color_map = jetcolormap(512);
a = get("current_axes");

//grayplot((xx), log(tt), (m) );
//a.log_flags='nl';
//abort;

plot((xx),    sum(m,'c'),'k');
plot((xx),    sum(m2,'c'),'b');

//plot((tt),    m(xamount,:),'k');
//plot((tt),    m2(xamount,:),'b');


