HestonEuropeanOption<- function(type, underlying, strike, dividendYield,
                                riskFreeRate, maturity, volatility,
                                v0, kappa, theta, sigma, rho) {
    type <- match.arg(type, c("call", "put"))
    val <- HestonEuropeanOptionEngine(type, underlying, strike, dividendYield, riskFreeRate, maturity, volatility, v0, kappa, theta, sigma, rho)
    #class(val) <- c("EuropeanOption", "Option")
    val
}

