function funs = brusselator_1d_functions
% predator-prey functions necessary in order
% to generate the Maxwell line conditions
% u_plus and VV


funs.f_of_uv = @f_of_XY;
funs.g_of_uv = @g_of_XY;
funs.df_du = @df_dX;
funs.df_dv = @df_dY;
funs.dg_du = @dg_dX;
funs.dg_dv = @dg_dY;

funs.ode_implicit_solver = @ode_implicit_solver;
funs.rhs_ode_solver = @rhs_ode_solver;
funs.plot_3d = @plot_3d;

function y = f_of_XY(X,Y,a,b,c,d)

  y = a - b*X + c*X.^2.*Y - d*X;

function y = g_of_XY(X,Y,b,c)

  y = b*X - c*X.^2.*Y;
  
function y = df_dX(X,Y,b,c,d)
  
  y = -b + 2*c*X.*Y - d;
  
function y = df_dY(X,c)

  y = c*X.^2;
  
function y = dg_dX(X,Y,b,c)

  y = b - 2*c*X.*Y;

function y = dg_dY(X,c)

  y = -c*X.^2;

%% solver and 3d plotter
  
function [uu,vv] = ode_implicit_solver(f_handle, g_handle,...
    uv0, t, x, params)
  
    dt = t(2) - t(1); 
    dx = x(2) - x(1);
    N = length(uv0);
    uu = zeros(N,length(t));
    vv = zeros(N,length(t));
    uu(:,1) = uv0(:,1);
    vv(:,1) = uv0(:,2);
    data = ones(N,3); data(:,2) = -2; lap = spdiags(data,-1:1,N,N); 
    lap(1,2) = 2; lap(N,N-1) = 2; % Neumann BCs
    
    a = params(1); b = params(2);
    du = params(3); dv = params(4); tau = params(5);
    au = speye(N)*(1/dt) - du/dx^2*lap;
    av = speye(N)*(tau/dt) - dv/dx^2*lap;

    for i = 2:length(t)
        bu = uu(:,i-1)/dt + f_handle(uu(:,i-1),vv(:,i-1),a);
        uu(:,i) = au\bu;
        bv = tau*vv(:,i-1)/dt + g_handle(uu(:,i-1),vv(:,i-1),b);
        vv(:,i) = av\bv;
%         norm_err = norm(uu(:,i) - uu(:,i-1),'inf');
    end
    
    
function duv_dt = rhs_ode_solver(t, y, f_handle, g_handle, params)
% RHS for solving a reaction difusion equation with one of matlab's
% ode solvers (ode15s for example). The original pdes are:
% u_t     = du*u_xx + f(u,v)
% tau*v_t = dv*v_xx + g(u,v),
% solved on a domain of length 2L,
   
  N = length(y)/2;
  u = y(1:N);
  v = y(N+1:2*N);

  u0 = u(2); u1 = u(N-1); % Neumann (2nd order)
  v0 = v(2); v1 = v(N-1);

  a = params(1);   b = params(2);
  du = params(3); dv = params(4); tau = params(5);
  L = params(6);  dx = params(7);

  cu = du/(2*L*dx)^2;
  cv = dv/(2*L*dx)^2/tau;

  f = f_handle(u,v,a);
  g = g_handle(u,v,b)/tau;

  dudt = zeros(N,1);
  dvdt = zeros(N,1);

  dudt(1) = (u0-2*u(1)+u(2))*cu + f(1);
  dudt(N) = (u(N-1)-2*u(N)+u1)*cu + f(N);

  dvdt(1) = (v0-2*v(1)+v(2))*cv + g(1);
  dvdt(N) = (v(N-1)-2*v(N)+v1)*cv + g(N);

  for i=2:N-1
    dudt(i) = (u(i-1)-2*u(i)+u(i+1))*cu + f(i);
    dvdt(i) = (v(i-1)-2*v(i)+v(i+1))*cv + g(i);
  end

  duv_dt = [dudt; dvdt];
  
function plot_3d(x, t, A, position)
  surf(x, t, A)
  rotate3d on
  shading interp
  if position
    azimut = position(1);
    elevation = position(2);
    view(azimut, elevation)
  end
