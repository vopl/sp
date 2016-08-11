x=0:0.001:15;
pow=10;
y=cos(2*%pi*x.^1/1) .* (cos(%pi*x/10)+1)/2;

clf;
plot(x,y);
