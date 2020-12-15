from utils import mkdir_p
import pandas

datapath = '../../../data/'
processeddata = '../../../data/processed/'

Teams = pandas.read_csv(datapath + 'Teams.csv')
TeamsDict = Teams.to_dict()
TeamObjects = {TeamsDict['Team_Id'][k] : {'Team_Name' : TeamsDict['Team_Name'][k]} for k in TeamsDict['Team_Id']}
# print(TeamObjects)

def processResults(df, features, output_path, filename):
    TeamObjects = {TeamsDict['Team_Id'][k] : {'Team_Name' : TeamsDict['Team_Name'][k]} for k in TeamsDict['Team_Id']}
    for season in range(2003,2016):
        for k in TeamObjects:
            for f in features:
                TeamObjects[k][f] = 0
            TeamObjects[k]['GameCount'] = 0

        for index, game in df[df.Season == season].iterrows():
            d = game.to_dict()
            Wteam = d['Wteam']
            Lteam = d['Lteam']

            for f in features:
                if f.startswith('W'):
                    TeamObjects[Wteam][f] += d[f.replace('Avg', '')]
                if f.startswith('L'):
                    TeamObjects[Lteam][f] += d[f.replace('Avg', '')]

            TeamObjects[Wteam]['GameCount'] += 1
            TeamObjects[Lteam]['GameCount'] += 1

        for k in TeamObjects:
            for f in features:
                if TeamObjects[k]['GameCount'] > 0:
                    TeamObjects[k][f] /= TeamObjects[k]['GameCount']

        TeamStats = pandas.DataFrame.from_dict(TeamObjects, orient='index')
        mkdir_p(output_path)
        TeamStats.to_csv(output_path + filename + str(season) + '.csv')
        print('Wrote out ' + output_path + filename + str(season) + '.csv')

def getAccuracy(df, features, output_path, filename):
    TeamObjects = {TeamsDict['Team_Id'][k] : {'Team_Name' : TeamsDict['Team_Name'][k]} for k in TeamsDict['Team_Id']}
    for season in range(2003,2016):
        for k in TeamObjects:
            for f in features:
                TeamObjects[k][f] = 0
            TeamObjects[k]['GameCount'] = 0
            TeamObjects[k]['Wfg_acc'] = 0
            TeamObjects[k]['Wfg3_acc'] = 0
            TeamObjects[k]['Wft_acc'] = 0
            TeamObjects[k]['Lfg_acc'] = 0
            TeamObjects[k]['Lfg3_acc'] = 0
            TeamObjects[k]['Lft_acc'] = 0

        for index, game in df[df.Season == season].iterrows():
            d = game.to_dict()
            Wteam = d['Wteam']
            Lteam = d['Lteam']

            TeamObjects[Wteam]['GameCount'] += 1
            TeamObjects[Lteam]['GameCount'] += 1

            for f in features:
                if f.startswith('W'):
                    TeamObjects[Wteam][f] += d[f.replace('_tot', '')]
                if f.startswith('L'):
                    TeamObjects[Lteam][f] += d[f.replace('_tot', '')]

        for k in TeamObjects:
            try:
                TeamObjects[k]['Wfg_acc'] = 100.0 * TeamObjects[k]['Wfgm_tot'] / TeamObjects[k]['Wfga_tot']
                TeamObjects[k]['Wfg3_acc'] = 100.0 * TeamObjects[k]['Wfgm3_tot'] / TeamObjects[k]['Wfga3_tot']
                TeamObjects[k]['Wft_acc'] = 100.0 * TeamObjects[k]['Wftm_tot'] / TeamObjects[k]['Wfta_tot']
                TeamObjects[k]['Lfg_acc'] = 100.0 * TeamObjects[k]['Lfgm_tot'] / TeamObjects[k]['Lfga_tot']
                TeamObjects[k]['Lfg3_acc'] = 100.0 * TeamObjects[k]['Lfgm3_tot'] / TeamObjects[k]['Lfga3_tot']
                TeamObjects[k]['Lft_acc'] = 100.0 * TeamObjects[k]['Lftm_tot'] / TeamObjects[k]['Lfta_tot']
            except:
                pass

        TeamStats = pandas.DataFrame.from_dict(TeamObjects, orient='index')
        mkdir_p(output_path)
        TeamStats.to_csv(output_path + filename + str(season) + '.csv')
        print('Wrote out ' + output_path + filename + str(season) + '.csv')

def run():
    compact_features = ['WscoreAvg', 'LscoreAvg']
    detailed_features = ['WscoreAvg', 'WfgmAvg', 'WfgaAvg', 'Wfgm3Avg', 'Wfga3Avg', 'WftmAvg', 'WftaAvg', 'WorAvg', 'WdrAvg', 'WastAvg', 'WtoAvg', 'WstlAvg', 'WblkAvg', 'WpfAvg',
                    'LscoreAvg', 'LfgmAvg', 'LfgaAvg', 'Lfgm3Avg', 'Lfga3Avg', 'LftmAvg', 'LftaAvg', 'LorAvg', 'LdrAvg', 'LastAvg', 'LtoAvg', 'LstlAvg', 'LblkAvg', 'LpfAvg']
    accuracy_features = ['Wfga_tot', 'Wfga3_tot', 'Wfta_tot', 'Lfga_tot', 'Lfga3_tot', 'Lfta_tot', 'Wfgm_tot', 'Wfgm3_tot', 'Wftm_tot', 'Lfgm_tot', 'Lfgm3_tot', 'Lftm_tot']

    # output_path = processeddata + 'RegularSeasonTeamStats/'
    # filename = 'RegularSeasonCompactTeamStats'
    # df = pandas.read_csv(datapath + 'RegularSeasonCompactResults.csv')
    # processResults(df, compact_features, output_path, filename)
    # filename = 'RegularSeasonDetailedTeamStats'
    # df = pandas.read_csv(datapath + 'RegularSeasonDetailedResults.csv')
    # processResults(df, detailed_features, output_path, filename)
    output_path = processeddata + 'RegularSeasonExtraStats/'
    filename = 'RegularSeasonAccuracyStats'
    df = pandas.read_csv(datapath + 'RegularSeasonDetailedResults.csv')
    getAccuracy(df, accuracy_features, output_path, filename)

    # output_path = processeddata + 'TourneyTeamStats/'
    # filename = 'TourneyCompactTeamStats'
    # df = pandas.read_csv(datapath + 'TourneyCompactResults.csv')
    # processResults(df, compact_features, output_path, filename)
    # filename = 'TourneyDetailedTeamStats'
    # df = pandas.read_csv(datapath + 'TourneyDetailedResults.csv')
    # processResults(df, detailed_features, output_path, filename)
