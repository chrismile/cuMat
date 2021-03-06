import sys
import os
import os.path
import json
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
from matplotlib.patches import Ellipse

"""
Arguments:
 path to the ground truth file as generated by 'GenerateData.py'
"""

def get_numbers(file):
    with open(file) as input_file:
        for line in input_file:
            line = line.strip()
            for number in line.split():
                yield number

def draw_ellipse(position, covariance, ax=None, **kwargs):
    """Draw an ellipse with a given position and covariance"""
    ax = ax or plt.gca()
    
    # Convert covariance to principal axes
    if covariance.shape == (2, 2):
        U, s, Vt = np.linalg.svd(covariance)
        angle = np.degrees(np.arctan2(U[1, 0], U[0, 0]))
        width, height = 2 * np.sqrt(s)
    else:
        angle = 0
        width, height = 2 * np.sqrt(covariance)
    
    # Draw the Ellipse
    for nsig in range(1, 4):
        ax.add_patch(Ellipse(position, nsig * width, nsig * height,
                             angle, **kwargs))

if __name__== "__main__":

    # parse ground truth
    groundTruthFile = sys.argv[1]
    print('Read ground truth data from',groundTruthFile)
    groundTruthNumbers = get_numbers(groundTruthFile);
    dimensions = int(next(groundTruthNumbers))
    components = int(next(groundTruthNumbers))
    numPoints = int(next(groundTruthNumbers))
    gWeights = np.zeros((components,), dtype=float)
    gMeans = np.zeros((components, dimensions), dtype=float)
    gCovariance = np.zeros((components, dimensions, dimensions), dtype=float)
    gPoints = np.zeros((numPoints, dimensions), dtype=float)
    for ic in range(components):
        gWeights[ic] = float(next(groundTruthNumbers))
        for x in range(dimensions):
            gMeans[ic,x] = float(next(groundTruthNumbers))
        for x in range(dimensions):
            for y in range(dimensions):
                gCovariance[ic,x,y] = float(next(groundTruthNumbers))
    for i in range(numPoints):
        for x in range(dimensions):
            gPoints[i,x] = float(next(groundTruthNumbers))

    print('weights:', gWeights)
    print('means:',gMeans)
    print('covariances:',gCovariance)

    # parse initial
    initialFile = sys.argv[2]
    print('Read ground truth data from',initialFile)
    initialNumbers = get_numbers(initialFile);
    _ = int(next(initialNumbers))
    _ = int(next(initialNumbers))
    _ = int(next(initialNumbers))
    initialWeights = np.zeros((components,), dtype=float)
    initialMeans = np.zeros((components, dimensions), dtype=float)
    initialCovariance = np.zeros((components, dimensions, dimensions), dtype=float)
    for ic in range(components):
        initialWeights[ic] = float(next(initialNumbers))
        for x in range(dimensions):
            initialMeans[ic,x] = float(next(initialNumbers))
        for x in range(dimensions):
            for y in range(dimensions):
                initialCovariance[ic,x,y] = float(next(initialNumbers))

    # parse results
    resultFile = sys.argv[3]
    print('Read results from', resultFile)
    with open(resultFile, 'r') as f:
        results = json.load(f)
    backends = ["Eigen", "CuMat"]

    #Create plot
    fig, axes = plt.subplots(2, max(2, len(backends)))

    #display ground truth
    colors = cm.rainbow(np.linspace(0, 1, components))
    axes[0,0].plot(gPoints[:, 0], gPoints[:, 1], ".", color='black', zorder=0)
    for cluster, color in zip(range(components), colors):
        axes[0,0].plot(gMeans[cluster, 0], gMeans[cluster, 1], ".", color=color, zorder=1)
        draw_ellipse(gMeans[cluster,:], gCovariance[cluster,:,:], color=color, zorder=-1, alpha=gWeights[cluster], ax=axes[0,0]);
    axes[0,0].set_title('Ground Truth')

    #display initial setting
    axes[0,1].plot(gPoints[:, 0], gPoints[:, 1], ".", color='black', zorder=0)
    for cluster, color in zip(range(components), colors):
        axes[0,1].plot(initialMeans[cluster, 0], initialMeans[cluster, 1], ".", color=color, zorder=1)
        draw_ellipse(initialMeans[cluster,:], initialCovariance[cluster,:,:], color=color, zorder=-1, alpha=initialWeights[cluster], ax=axes[0,1]);
    axes[0,1].set_title('Initial components')

    # display results
    for i,backend in enumerate(backends):
        axes[1,i].plot(gPoints[:, 0], gPoints[:, 1], ".", color='black', zorder=0)
        for cluster, color in zip(range(components), colors):
            weight = results[backend]['Components'][cluster]['Weight']
            mean = np.array(results[backend]['Components'][cluster]['Mean'])
            cov = np.reshape(np.array(results[backend]['Components'][cluster]['Cov']), (dimensions, dimensions))

            axes[1,i].plot(mean[0], mean[1], ".", color=color, zorder=1)
            draw_ellipse(mean, cov, color=color, zorder=-1, alpha=weight, ax=axes[1,i]);
        axes[1,i].set_title(backend)


    #plt.show()
    plt.savefig(os.path.splitext(resultFile)[0]+'.png', bbox_inches='tight', dpi=300)

#plt.savefig(setPath+'.png', bbox_inches='tight', dpi=300)
