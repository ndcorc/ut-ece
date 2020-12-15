M = 0.25;
m = 0.1;
b = 0.1;
I = 0.006;
g = 9.81;
l = 0.1651;

p = I*(M+m)+M*m*l^2; %denominator for the A and B matrices

A = [0      1              0           0;
     0 -(I+m*l^2)*b/p  -(m^2*g*l^2)/p   0;
     0      0              0           1;
     0 (m*l*b)/p       m*g*l*(M+m)/p  0];

B = [     0;
     (I+m*l^2)/p;
          0;
        -(m*l)/p];

C = [1 0 0 0;
     0 0 1 0];

D = [0;
     0];

states = {'x' 'x_d' 'theta' 'theta_d'};
inputs = {'u'};
outputs = {'x'; 'theta'};

sys_ss = ss(A,B,C,D,'statename',states,'inputname',inputs,'outputname',outputs);

poles = eig(A)


Q = C'*C;
Q(1,1) = 5000;
Q(3,3) = 100
R = 1;
K = lqr(A,B,Q,R)
