xValuesVector1 = c(38.3,6.2,3.7,2.6,2.1,14.6,5.6,3.7,2.3,2.0,11.9,5.5,3.4,2.2,2.0,6.6,4.6,3.1,2.2,1.9,6.3,4.5,2.7,2.1,1.8) 
media = mean(xValuesVector1)
mediana = median(xValuesVector1)

#quartz() abre uma nova janela sem ser a do plots
histograma = hist(xValuesVector1,
                  include.lowest = TRUE, right = TRUE, fuzz = 1e-7,
                  density = NULL, angle = 45, col = "lightgreen", border = NULL,
                  main = paste(""),ylim = NULL,
                  xlab = "sida", ylab = "Frequência absoluta",
                  axes = TRUE, plot = TRUE, labels = FALSE, warn.unused = TRUE)


maximo = max(xValuesVector1)
minimo = min(xValuesVector1)

amplitude = maximo - minimo
amplitude2 = range(xValuesVector1)

desvio_p = sd(xValuesVector1)


caixa = boxplot(xValuesVector1)

