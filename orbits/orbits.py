from pylab import show, polar, figure, gca, NullLocator, legend, savefig
from matplotlib.patches import Circle
from numpy import cos, linspace, pi,ones

#params = [ [0.723, 0.006], # Venus
#           [1.530, 0.093], # Mars
#           [1.000, 0.016]  # Earth
#         ]

params = [ [0.723, 0.006, "Venus"],
           [1.000, 0.0,   "Earth"],
           [1.530, 0.0,   "Mars"],
         ]

def mktransit(a0,a1):
    m,M = min(a0,a1), max(a0,a1)
    return [ (a0+a1)/2., (1.-m/M) / (1.+m/M) ]

params.append(mktransit(params[2][0], 4.0)          + ['Uninhabitable'])
params.append(mktransit(params[0][0], 0.5)          + ['Uninhabitable'])
params.append(mktransit(params[1][0], params[2][0]) + ['Transit'])
params.append(mktransit(params[1][0], params[0][0]) + ['Transit'])

figure(figsize=(12,12), facecolor='cyan', edgecolor='green')

for a,e,label in params:
    theta = linspace(0,2*pi, 100)
    r = a * (1-e**2) / (1 - e*cos(theta))
    polar(theta, r, label=label)

gca().fill_between(linspace(0.0, 2*pi,100), 0.65*ones(100), color='yellow')

gca().xaxis.set_major_locator(NullLocator())
gca().yaxis.set_major_locator(NullLocator())
#gca().set_ylim(0,1.8)
legend(loc='lower left', bbox_to_anchor = (-0.1, -0.0))

savefig('orbits-big.pdf', facecolor='cyan')

show()

