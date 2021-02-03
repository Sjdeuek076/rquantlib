library(RQuantLib)
EuropeanOption("call", 100, 100, 0.01, 0.03, 0.5, 0.4)
HestonEuropeanOption("call", 100, 100, 0.01, 0.03, 0.5, 0.4, 0.1, 0.2, 0.3, 0.4, 0.5)
HestonEuropeanOptionEngine(type, underlying, strike, dividendYield, riskFreeRate, maturity, volatility, v0, kappa, theta, sigma, rho)
EuropeanOption