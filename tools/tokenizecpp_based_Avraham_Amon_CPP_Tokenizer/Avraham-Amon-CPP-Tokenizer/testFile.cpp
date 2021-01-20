int x,y,z;
int d;
char c;

cin>>c;
cout<<c;

if(c == 'Z'){
cout<<1;
}
else
cout<<0;

cin>>x>>y;

z=x+2*y-7;

d = z / (x + 2 * x);

cout<<x<<y+3*z<<d-(x-2);

while(y>0){
z=x % y;
x=y;
y=z;
}
cout<<x;