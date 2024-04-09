# Data requirements and description
## rr_obs_daily.csv
Multisite daily rainfall dataset to be disaggregated
* first 3 column: year, month and day
* other columnd: daily rainfall series in site order

## rr_obs_hourly.csv
Historical observed multisite hourly rainfall dataset
* the number of rainfall sites should be the same as that in rr_obs_daily.csv
* first 4 column: year, month, day and hour
* the other columns: hourly rainfall data series

## cp_series.csv
Circulation pattern (CP) classifications
* file type: Tab-separated values file without header
* first 3 column: year, month and day
* 4th column: CP classes; datatype: integer
