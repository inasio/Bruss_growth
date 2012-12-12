function bruss_simple_1dplot  % plot simple mesa figures
%% constants

N = 100;
L = 1; % length of the domain
H = 2*L/(N-1); % 100 grid points 
x = -L:H:L; x = x';
n = 20;
Dx = 0.005;
Dy = n*Dx; % diffusion coefficient ratio, in the v equation

Tend = 50; % integration time
Dt = .005; % time step size, arbitrary for nonlinear problems

a = 0.05;
b = 0.5;
c = 1.8;
d = 0.05;

%% non-dimensionalization stuff
Dx = Dx/d;
Dy = Dy/d;
a = a*c/(d*sqrt(d));
b = b/d;
c = 1;
d = 1;

%% Lacalli parameters
k_1 = b-d;
k_2 = a^2*c/d^2;
k_3 = -b;
k_4 = -a^2*c/d^2
k_cr = sqrt(-k_2*k_3);
k_1p = k_1/k_cr;
k_4p = k_4/k_cr;

%% Lacalli-Turing plot
K_1p = 0.1:0.01:2;
condition_a = n*K_1p - 2*sqrt(n);
condition_b = K_1p - (n+1)/sqrt(n);
condition_c = K_1p - 2;
condition_d = -K_1p;
condition_e = K_1p - 4*sqrt(n)/(n+1);
condition_f = -1./K_1p;
figure(1)
plot(K_1p,condition_a,K_1p,condition_b,K_1p,condition_c,K_1p,condition_d,...
  K_1p,condition_e,K_1p,condition_f), hold on
axis([0,1.2,-7,0])
plot(k_1p,k_4p,'*')


%% nonlinear function definitions   
F_bruss = inline('a - b*X + c*X.^2.*Y - d*X','X','Y','a','b','c','d');
G_bruss = inline('b*X - c*X.^2.*Y','X','Y','b','c');

%% initial conditions
X_homog = a/d
Y_homog = b*d/(a*c)
%   XY_0(:,1) = 0.37*(tanh(.3*(.4*L-x)/Ep) + tanh(.3*(x+.4*L)/Ep));
%   XY_0(:,2) = 0.3251*ones(N,1); % initial conditions

%   XY_0(:,1) = X_homog*(ones(N,1) + 0.05*randn(N,1));
%   XY_0(:,2) = Y_homog*(ones(N,1) + 0.05*randn(N,1));; % initial conditions

XY_0(:,1) = (cos(2*pi*x) + 1 + 0.01*randn(N,1))*X_homog/1.5 + 1.5;
XY_0(:,2) = (1 - cos(2*pi*x) + 0.01*randn(N,1))*Y_homog/6 + 1;


%% compute stationary solution
% [XX0,YY0,TT] = XY_stationary_imex(XY_0,Tend, 1.0);

[TT,XY] = ode15s(@XY_stationary_explicit, 0:0.1:Tend, XY_0);
XX0 = XY(:,1:N)';
YY0 = XY(:,N+1:2*N)';

%% plotting
figure(2)
surf(x,TT,XX0') 
shading interp, view(58,42)

figure(3)
surf(x,TT,YY0') 
shading interp, view(58,42)

figure(4)
plot(x, XX0(:,end)'), hold on

function [xx,yy,t] = XY_stationary_imex(xy_0,t_end,dt)  % y = [X,Y,T]
  xx = zeros(length(xy_0),round(t_end/dt));
  yy = zeros(length(xy_0),round(t_end/dt));
  xx(:,1) = xy_0(:,1);
  yy(:,1) = xy_0(:,2);
  data = ones(N,3); data(:,2) = -2; lap = spdiags(data,-1:1,N,N); 
  lap(1,2) = 2; lap(N,N-1) = 2; % Neumann BCs
  ax = speye(N)*(1/dt+1) - Dx/H^2*lap;
  ay = speye(N)*(1/dt+1) - Dy/H^2*lap;

%     i = 2; norm_err = 1;
%     while norm_err > 1e-6
  for i = 2:round(t_end/dt)
      bx = xx(:,i-1)/dt + F_bruss(xx(:,i-1),yy(:,i-1),a,b,c,d);
      xx(:,i) = ax\bx;
      by = yy(:,i-1)/dt + G_bruss(xx(:,i),yy(:,i-1),b,c);
      yy(:,i) = ay\by;
%         norm_err = norm(uu(:,i) - uu(:,i-1),'inf');
  end
  t = dt:dt:i*dt;
end

function dfdt = XY_stationary_explicit(t,y)

  xx = y(1:N);
  yy = y(N+1:2*N);
  
  L2 = (2*L)^2;
  cx = Dx/(L2*H^2);
  cy = Dy/(L2*H^2);

  dxdt = zeros(N,1);
  dydt = zeros(N,1);

  dxdt(1) = cx*(xx(2) - xx(1)) + F_bruss(xx(1),yy(1),a,b,c,d);
  dxdt(N) = cx*(xx(N-1) - xx(N)) + F_bruss(xx(N),yy(N),a,b,c,d);

  dydt(1) = cy*(yy(2) - yy(1)) + G_bruss(xx(1),yy(1),b,c);
  dydt(N) = cy*(yy(N-1) - yy(N)) + G_bruss(xx(N),yy(N),b,c);

  for i=2:N-1
    dxdt(i) = cx*(xx(i-1)-2*xx(i)+xx(i+1)) + F_bruss(xx(i),yy(i),a,b,c,d);
    dydt(i) = cy*(yy(i-1)-2*yy(i)+yy(i+1)) + G_bruss(xx(i),yy(i),b,c);
  end

  dfdt = [dxdt; dydt];

  end
end
