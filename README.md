

## Algorithm

- k-nearest neighbor sampling
- method-of-fragments
- continuity: 1 day or 3 days
- conditioned on seasonality (12 months) or circulation patterns
- similarity measure: Structural Similarity Index Measure (SSIM)
- flexible wet-dry status filtering; borrow fragments from rainy neighbors


## Evaluation

* evaluate at annual scale, rather than year by year. 
* compare with kNN_MOF_cp (NOT)
* apply the model at gridded based domain, like Eobs or RADOLAN
* site scale: skewness, lag-1 autocorrelation, mean, standard deviation, intersite correlation, extremes
* areal scale: aggregtaed to areal average rainfall 
* WEI scale: spatially and temporally integrated index


