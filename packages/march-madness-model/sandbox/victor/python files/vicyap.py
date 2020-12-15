import pandas
import csv

datapath = '../../data/'
processeddata = '../../data/processed/'

"""
Columns:
    Team_Id
    Team_Name
"""
Teams = pandas.read_csv(datapath + 'Teams.csv')
# print(Teams.head())

assert Teams['Team_Id'][0] == 1101
assert Teams['Team_Name'][0] == 'Abilene Chr'

"""
Columns:
    Season - this is the year of the associated entry in seasons.csv
    Daynum - this integer always ranges from 0 to 132, and tells you what day the game was played on. It represents an offset from the "dayzero" date in the "seasons.csv" file.
    Wteam -  this identifies the id number of the team that won the game, as listed in the "teams.csv" file. No matter whether the game was won by the home team or visiting team, "wteam" always identifies the winning team.
    Wscore - this identifies the number of points scored by the winning team.
    Lteam - this identifies the id number of the team that lost the game.
    Lscore -  this identifies the number of points scored by the losing team.
    Wloc -  this identifies the "location" of the winning team. If the winning team was the home team, this value will be "H". If the winning team was the visiting team, this value will be "A". If it was played on a neutral court, then this value will be "N".
    Numot - this indicates the number of overtime periods in the game, an integer 0 or higher.

    wfgm - field goals made
    wfga - field goals attempted
    wfgm3 - three pointers made
    wfga3 - three pointers attempted
    wftm - free throws made
    wfta - free throws attempted
    wor - offensive rebounds
    wdr - defensive rebounds
    wast - assists
    wto - turnovers
    wstl - steals
    wblk - blocks
    wpf - personal fouls

    lfgm - field goals made
    lfga - field goals attempted
    lfgm3 - three pointers made
    lfga3 - three pointers attempted
    lftm - free throws made
    lfta - free throws attempted
    lor - offensive rebounds
    ldr - defensive rebounds
    last - assists
    lto - turnovers
    lstl - steals
    lblk - blocks
    lpf - personal fouls
"""
RegularSeasonDetailedResults = pandas.read_csv(datapath + 'RegularSeasonDetailedResults.csv')
# print(RegularSeasonDetailedResults.head())

"""
Columns:
    Same as RegularSeasonDetailedResults
"""
# TourneyDetailedResults = pandas.read_csv(datapath + 'TourneyDetailedResults.csv')
# print(TourneyDetailedResults.head())

columns = ['Team_Id', 'Wscore', 'Lscore']

# SeasonTeamResults = pandas.DataFrame(columns=columns)
# print(SeasonTeamResults.head())
# for r in RegularSeasonDetailedResults.itertuples(name=None):
#     print(r)
#     winningTeamId = r[3]
#     Teams.loc[Teams['Team_Id'] == winningTeamId]
#     SeasonTeamResults.loc[SeasonTeamResults.Team_Id == winningTeamId]

WinningTeamGames = pandas.merge(left=Teams, right=RegularSeasonDetailedResults, left_on=['Team_Id'], right_on=['Wteam'])
# WinningTeamGames.to_csv(processeddata + 'WinningTeamGames.csv')
season = 2014
WinningTeamSingleSeason = WinningTeamGames[WinningTeamGames['Season'] == season]
# WinningTeamSingleSeason.to_csv(processeddata + 'WinningGames2014.csv')

# WinningTeamAverages = pandas.DataFrame(columns=WinningTeamSingleSeason.columns.values)
WinningTeamAverages = pandas.DataFrame(columns=WinningTeamSingleSeason.columns.values)
i = 0
for id in Teams['Team_Id']:
    team = WinningTeamSingleSeason[WinningTeamSingleSeason['Team_Id'] == id]
    # team['Wscore'] = team['Wscore'].mean()
    WinningTeamAverages.loc[i] = [team[x].mean() for x in team]
    i = i + 1
    print(i)
WinningTeamAverages.to_csv(processeddata + str(season) + 'Winners.csv')

LosingTeamGames = pandas.merge(left=Teams, right=RegularSeasonDetailedResults, left_on=['Team_Id'], right_on=['Lteam'])
# LosingTeamGames.to_csv(processeddata + 'LosingTeamGames.csv')

"""
one csv is per season
Per Season
    one row per Regular Teams
        Associated Features
    one row per Tournament Teams
        Associated Features
"""
# index = Teams['Team_Id']
# columns = ['Team_Name']
# Season_csv = pandas.DataFrame(data=Teams)
# Season_csv.to_csv(processeddata + 'Season.csv')

