stacksize('max');

h = 55;
w = -1;
name='xag10';
z = read("P:\finance\projects\nhel\16.harmonicSub\console\'+name+'\tans", w, h);
z2 = read("P:\finance\projects\nhel\16.harmonicSub\console\'+name+'\tans2", -1, 1);

w = size(z,1);
x = 1:w;
y = 1:h;

//z = 1-abs(z/2);


//scf;
clf;

if 01
  scf(0);
  clf;
  f = gcf();
  f.color_map = jetcolormap(100);
  colorbar(min(z),max(z));
  //colorbar(0.15,0.15);
  //z(1,1)=-0.15;
  //z(1,2)=0.15;
  grayplot(x,y,z);
  scf(1);
  clf;
  plot(z2);
  //plot(z(w-1, 1:h));
  //plot(z(:, [1:1]));
//  plot(abs(fft(z(100:20000, 1))));
  
  a=get("current_axes");
  a.grid = [0,0,0];
  
else  plot(z);
  plot(z(1:w, 30));
//  plot(fftshift(abs(fft(z(1:w, 1)))));
 
  plot(z(w-1, 1:h));
  a=get("current_axes");
  a.grid = [0,0,0];
end;

