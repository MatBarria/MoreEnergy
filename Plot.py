import ROOT
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import mplhep as hep

# from matplotlib import colorsp
plt.style.use(hep.style.ATLAS)  # or ATLAS/LHCb2
#hep.style.use(["ATLAS", "fira", "firamath"])

inputDirectory  = "~/proyecto/MoreEnergy/Data/"
outputDirectory = "/home/matias/proyecto/MoreEnergy/Plots/"

#Shift the data on Zh to make the plot more readability
ZhShift = 0.015
Q2Shift = 0.08
NuShift = 0.08

# Upper limmit in the y axis
FullYlimit = 0.08
Q2Ylimit   = 0.08
NuYlimit   = 0.08
ZhYlimit   = 0.15


def PtBroadZhTarSplit():

    fig, axs = plt.subplots(1, 3, sharey = 'row', sharex = 'col')
    width  = 16 #7.056870070568701
    height = 4
    fig.set_size_inches(width, height)

    fig.subplots_adjust(left = None, bottom = None, right = None, top = None, wspace = 0.02, hspace = 0.02)

    file = ROOT.TFile.Open(inputDirectory + "Pt_broad_Zh.root", "READ")

    tarList   = ["C", "Fe", "Pb"]
    colorList = ["red", "Blue", "black"]
    labelList = ["One $\pi +$", "Two $\pi+$", "Three $\pi +$"]

    for i in range(3): # Loops on the diffrent targets
        axs[i].set_ylim(0, ZhYlimit)
        axs[i].set_xlim(0.075, 1.03)
        for j in range(3): # Loops on the number of pions
            graphName = "PtBroad_Zh_" + tarList[i] + "_" + str(j)
            graph     = file.Get(graphName)
            # Extrac the data from the TGraph
            nPoints = graph.GetN()
            x  = np.ndarray(nPoints, dtype = float, buffer = graph.GetX())
            x  = x + (-ZhShift + ZhShift*j) # Shit the data for readability
            y  = np.ndarray(nPoints, dtype = float, buffer = graph.GetY())
            ey = np.ndarray(nPoints, dtype = float, buffer = graph.GetEY())
            # Generate the plot
            axs[i].errorbar(x, y, ey, marker = "o", linestyle = "", markerfacecolor = colorList[j],
                            color = colorList[j], markersize = 6, label = labelList[j])

    # Set the labels for the three plots
    axs[0].set_ylabel(r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc = "center", fontsize = 15)
    axs[0].set_xlabel(r'$Z_\mathrm{h}$', fontsize = 14)
    axs[1].set_xlabel(r'$Z_\mathrm{h}$', fontsize = 14)
    axs[2].set_xlabel(r'$Z_\mathrm{h}$', fontsize = 14)

    axs[0].annotate(r'Carbon', xy = (0.04, 1.04), xycoords = 'axes fraction', fontsize = 12)
    axs[1].annotate(r'Iron',   xy = (0.04, 1.04), xycoords = 'axes fraction', fontsize = 12)
    axs[2].annotate(r'Lead',   xy = (0.04, 1.04), xycoords = 'axes fraction', fontsize = 12)

    axs[0].legend(frameon = False, loc = 'upper left', fontsize = 11)

    fig.show()

    fig.align_ylabels(axs[:])
    fig.savefig(outputDirectory + "PtBroad_Zh_Target.pdf", bbox_inches = 'tight')
    print(outputDirectory + "PtBroad_Zh_Target.pdf Has been created")



def PtBroadZhNPionSplit():

    fig, axs = plt.subplots(1, 3, sharey = 'row', sharex = 'col')
    width  = 16 #7.056870070568701
    height = 4
    fig.set_size_inches(width, height)

    fig.subplots_adjust(left = None, bottom = None, right = None, top = None, wspace = 0.02, hspace = 0.02)

    file = ROOT.TFile.Open(inputDirectory + "Pt_broad_Zh.root", "READ")

    tarList   = ["C", "Fe", "Pb"]
    colorList = ["red", "Blue", "black"]
    #labelList = ["C", "Fe", "Pb"]

    for i in range(3): # Loops on the diffrent targets
        for j in range(3): # Loops on the number of pions
            axs[j].set_ylim(0, ZhYlimit)
            axs[j].set_xlim(0.075, 1.03)
            graphName = "PtBroad_Zh_" + tarList[i] + "_" + str(j)
            graph     = file.Get(graphName)
            nPoints = graph.GetN()
            x  = np.ndarray(nPoints, dtype = float, buffer = graph.GetX())
            x  = x + (-ZhShift + ZhShift*i)
            y  = np.ndarray(nPoints, dtype = float, buffer = graph.GetY())
            ey = np.ndarray(nPoints, dtype = float, buffer = graph.GetEY())
            axs[j].errorbar(x, y, ey, marker = "o", linestyle = "", markerfacecolor = colorList[i],
                            color = colorList[i], markersize = 6, label = tarList[i])

    axs[0].set_ylabel(r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc = "center", fontsize = 15)
    axs[0].set_xlabel(r'$Z_\mathrm{h}$', fontsize = 14)
    axs[1].set_xlabel(r'$Z_\mathrm{h}$', fontsize = 14)
    axs[2].set_xlabel(r'$Z_\mathrm{h}$', fontsize = 14)

    axs[0].annotate(r'One Pion',     xy = (0.04, 1.04), xycoords = 'axes fraction', fontsize = 15)
    axs[1].annotate(r'Two Pion',     xy = (0.04, 1.04), xycoords = 'axes fraction', fontsize = 15)
    axs[2].annotate(r'Three Pion',   xy = (0.04, 1.04), xycoords = 'axes fraction', fontsize = 15)

    axs[0].legend(frameon = False, loc = 'upper left', fontsize = 11)

    fig.show()

    fig.align_ylabels(axs[:])
    fig.savefig(outputDirectory + "PtBroad_Zh_NPion.pdf", bbox_inches = 'tight')
    file.Close()
    print(outputDirectory + "PtBroad_Zh_NPion.pdf Has been created")


def PtBroadFullIntegrated():

    fig, axs = plt.subplots(1, 1, constrained_layout = True)
    # For one column
    width  = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    file = ROOT.TFile.Open(inputDirectory + "Pt_broad_FullIntegrated.root", "READ")

    tarList    = ["C", "Fe", "Pb"]
    colorList  = ["red", "Blue", "black"]
    nPionList  = ["One $\pi +$", "Two $\pi+$", "Three $\pi +$"]
    markerList = ["o", "s", "D"]

    for i in range(3): # Loops on the diffrent targets
        for j in range(3): # Loops on the number of pions
            axs.set_ylim(0, FullYlimit)
            axs.set_xlim(1.5, 6.5)
            graphName = "PtBroad_FullIntegrated_" + tarList[i] + "_" + str(j)
            graph     = file.Get(graphName)
            if j == 0:
                axs.errorbar(graph.GetX(), graph.GetY(), graph.GetEY(),
                            marker = markerList[j], linestyle = "", markerfacecolor = colorList[i], color = colorList[i],
                            markersize = 6, label = tarList[i])
            if i == 2:
                axs.errorbar(graph.GetX(), graph.GetY(), graph.GetEY(),
                            marker = markerList[j], linestyle = "", markerfacecolor = "grey", color = colorList[i],
                            markersize = 6, label = nPionList[j])

            axs.errorbar(graph.GetX(), graph.GetY(), graph.GetEY(),
                            marker = markerList[j], linestyle = "", markerfacecolor = colorList[i], color = colorList[i],
                            markersize = 6, label = None)



    axs.set_ylabel(r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc = "center", fontsize = 15)
    axs.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

    axs.set_xlabel(r'$A^\mathrm{\frac{1}{3}}$', loc = "center", fontsize = 14)


    axs.legend(ncol = 2, frameon = False, loc = 'upper left', fontsize = 11)

    fig.savefig(outputDirectory + "PtBroad_FullIntegrated.pdf", bbox_inches = 'tight')
    file.Close()
    print(outputDirectory + "PtBroad_FullIntegrated.pdf Has been created")


def PtBroadQ2():

    fig, axs = plt.subplots(1, 1, constrained_layout = True)
    # For one column
    width  = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    file = ROOT.TFile.Open(inputDirectory + "Pt_broad_Q2.root", "READ")

    tarList    = ["C", "Fe", "Pb"]
    colorList  = ["red", "Blue", "black"]
    nPionList  = ["One $\pi +$", "Two $\pi+$", "Three $\pi +$"]
    markerList = ["o", "s", "D"]

    for i in range(3): # Loops on the diffrent targets
        for j in range(3): # Loops on the number of pions
            axs.set_ylim(0, Q2Ylimit)
            axs.set_xlim(0.87, 3.5)
            graphName = "PtBroad_Q2_" + tarList[i] + "_" + str(j)
            graph     = file.Get(graphName)
            nPoints   = graph.GetN()
            x  = np.ndarray(nPoints, dtype = float, buffer = graph.GetX())
            x  = x + (-Q2Shift + Q2Shift*i)
            y  = np.ndarray(nPoints, dtype = float, buffer = graph.GetY())
            ey = np.ndarray(nPoints, dtype = float, buffer = graph.GetEY())
            if j == 0:
                axs.errorbar(x, y, ey, marker = markerList[j], linestyle = "", markerfacecolor = colorList[i],
                            color = colorList[i], markersize = 6, label = tarList[i])
            if i == 2:
                axs.errorbar(x, y, ey, marker = markerList[j], linestyle = "", markerfacecolor = "grey", color = colorList[i],
                            markersize = 6, label = nPionList[j])

            axs.errorbar(x, y, ey, marker = markerList[j], linestyle = "", markerfacecolor = colorList[i], color = colorList[i],
                        markersize = 6, label = None)



    axs.set_ylabel(r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc = "center", fontsize = 15)
    axs.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

    axs.set_xlabel(r'$Q^2 [GeV^{2}]$', loc = "center", fontsize = 14)


    axs.legend(ncol = 2, frameon = False, loc = 'upper left', fontsize = 11)

    fig.savefig(outputDirectory + "PtBroad_Q2-.pdf", bbox_inches = 'tight')
    file.Close()
    print(outputDirectory + "PtBroad_Q2-.pdf Has been created")



def PtBroadNu():

    fig, axs = plt.subplots(1, 1, constrained_layout = True)
    # For one column
    width  = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    file = ROOT.TFile.Open(inputDirectory + "Pt_broad_Nu.root", "READ")

    tarList    = ["C", "Fe", "Pb"]
    colorList  = ["red", "Blue", "black"]
    nPionList  = ["One $\pi +$", "Two $\pi+$", "Three $\pi +$"]
    markerList = ["o", "s", "D"]

    for i in range(3): # Loops on the diffrent targets
        for j in range(3): # Loops on the number of pions
            axs.set_ylim(0, NuYlimit)
            axs.set_xlim(2.5, 4.26)
            graphName = "PtBroad_Nu_" + tarList[i] + "_" + str(j)
            graph     = file.Get(graphName)
            nPoints   = graph.GetN()
            x  = np.ndarray(nPoints, dtype = float, buffer = graph.GetX())
            x  = x + (-Q2Shift + Q2Shift*i)
            y  = np.ndarray(nPoints, dtype = float, buffer = graph.GetY())
            ey = np.ndarray(nPoints, dtype = float, buffer = graph.GetEY())
            if j == 0:
                axs.errorbar(x, y, ey, marker = markerList[j], linestyle = "", markerfacecolor = colorList[i],
                            color = colorList[i], markersize = 6, label = tarList[i])
            if i == 2:
                axs.errorbar(x, y, ey, marker = markerList[j], linestyle = "", markerfacecolor = "grey", color = colorList[i],
                            markersize = 6, label = nPionList[j])

            axs.errorbar(x, y, ey, marker = markerList[j], linestyle = "", markerfacecolor = colorList[i], color = colorList[i],
                        markersize = 6, label = None)



    axs.set_ylabel(r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc = "center", fontsize = 15)
    axs.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

    axs.set_xlabel(r'$\nu [GeV]$', loc = "center", fontsize = 14)


    axs.legend(ncol = 2, frameon = False, loc = 'upper left', fontsize = 11)

    fig.savefig(outputDirectory + "PtBroad_Nu-.pdf", bbox_inches = 'tight')
    file.Close()
    print(outputDirectory + "PtBroad_Nu-.pdf Has been created")

# Call funtions

PtBroadZhTarSplit()
PtBroadZhNPionSplit()
PtBroadFullIntegrated()
PtBroadQ2()
PtBroadNu()
