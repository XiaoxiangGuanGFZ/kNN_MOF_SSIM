library(ggplot2)
a = 1
u = 0.5
x = seq(0, 1, 0.02)
y = exp(a * abs(x - u)) - 0.8

df <- data.frame(x,y)

ggplot(
  data = df, aes(x = x, y = y)
) + geom_line() +
  scale_y_continuous(limits = c(0, 1.4))
  