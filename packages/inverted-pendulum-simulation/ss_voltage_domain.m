mc = 0.25;
mp = 0.1;
M = mc+mp;
b = 0.1;
I = 0.006;
g = 9.81;
l = 0.1651;
L = (I+mp*l^2)/(mp*l);
Km = 0.072e-3*60/(2*pi);
Kg = 6.25;
R = 3.9;
r = .001625;


A = [0      1              0           0;
     0 -(1/(M-(mp*l/L)))*((Km*Kg)^2/(R*r^2))  -(g*mp*l)/(L*(M-(mp*l/L)))   0;
     0      0              0           1;
     0  (1/(M*(L-(mp*l/M))))*((Km*Kg)^2/(R*r^2)) g/(L-(mp*l/M)) 0];

B = [     0;
     (1/(M-(mp*l/L)))*((Km*Kg)/(R*r));
     0;
     -(1/(M*(L-(mp*l/M))))*((Km*Kg)/(R*r))];

C = [1 0 0 0;
     0 0 1 0];

D = [0;
     0];

states = {'x' 'x_d' 'theta' 'theta_d'};
inputs = {'V'};
outputs = {'x'; 'theta'};

sys_ss = ss(A,B,C,D,'statename',states,'inputname',inputs,'outputname',outputs);

poles = eig(A)


Q = C'*C;
Q(1,1) = 5000;
Q(3,3) = 100
R = 1;
K = lqr(A,B,Q,R)
