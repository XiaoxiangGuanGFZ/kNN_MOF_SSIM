

## Algorithm

- k-nearest neighbor sampling
- method-of-fragments
- continuity: 1 day or 3 days
- conditioned on seasonality (12 months) or circulation patterns
- similarity measure: Structural Similarity Index Measure (SSIM)
- flexible wet-dry status filtering; 

the disaggregation strategy:

- borrow fragments from rainy neighbors
- disaggregate the multisite rainfall in a recursive manner


## Evaluation

* evaluate at annual scale, rather than year by year. 
* compare with kNN_MOF_cp (NOT)
* apply the model at gridded based domain, like Eobs or RADOLAN
* site scale: skewness, lag-1 autocorrelation, mean, standard deviation, intersite correlation, extremes
* areal scale: aggregtaed to areal average rainfall 
* WEI scale: spatially and temporally integrated index

## to-do list (ideas)

use SSIM as the similarity metric to select the candidate recursively until
all the rainy (wet) sites are disaggregated.

from this strategy, the mean and skewness are well reproduced in the disaggregation model.
but, the standard deviation, and the lag-1 autocorrelation are not well represented in the model.

so, how should we improve the model's performance in especially terms of sd.

continuity -> acf1
? -> sd

what if we apply the recursive disaggregation on gridd-based rainfall: RADOLAN

how deep the function goes to redistribute the daily multisite rainfall completely?
(depth)

