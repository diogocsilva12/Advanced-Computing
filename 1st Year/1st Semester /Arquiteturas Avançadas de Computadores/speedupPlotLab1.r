threads <- c(1, 2, 4, 5, 8, 10, 15,20)
times <- c(139.219, 74.504, 85.680, 64.052, 83.252, 76.858, 70.707,73.328)
speedup <- times[1] / times

plot(threads, speedup, type="b", pch=19, col="blue",
     main="Speedup vs Number of Threads",
     xlab="Number of Threads", ylab="Speedup (T₁/Tₙ)")


lines(threads, threads, lty=2, col="red")
legend("topleft", legend=c("Measured", "Ideal"), col=c("blue","red"),
       pch=c(19, NA), lty=c(1,2))