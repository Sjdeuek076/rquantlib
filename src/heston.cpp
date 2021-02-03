#include <rquantlib_internal.h>
#include <ql/instruments/dividendvanillaoption.hpp>
#include <ql/processes/hestonprocess.hpp>
#include <ql/models/equity/hestonmodel.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/period.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/termstructures/yield/zerocurve.hpp>
#include <ql/pricingengines/vanilla/analytichestonengine.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/quotes/simplequote.hpp>

// [[Rcpp::interfaces(r, cpp)]]

// [[Rcpp::export]]
Rcpp::List HestonEuropeanOptionEngine(std::string type,
                                double underlying,
                                double strike,
                                double dividendYield,
                                double riskFreeRate,
                                double maturity,
                                double volatility,
                                double v0,
                                double kappa,
                                double theta,
                                double sigma,
                                double rho) {

#ifdef QL_HIGH_RESOLUTION_DATE
    // in minutes
    boost::posix_time::time_duration length = boost::posix_time::minutes(boost::uint64_t(maturity * 360 * 24 * 60));
#else
    int length           = int(maturity*360 + 0.5); // FIXME: this could be better
#endif

    QuantLib::Option::Type optionType = getOptionType(type);
    QuantLib::Date today = QuantLib::Date::todaysDate();
    QuantLib::Settings::instance().evaluationDate() = today;

    // new framework as per QuantLib 0.3.5
    QuantLib::DayCounter dc = QuantLib::Actual360();
    QuantLib::ext::shared_ptr<QuantLib::SimpleQuote> spot(new QuantLib::SimpleQuote( underlying ));
    QuantLib::ext::shared_ptr<QuantLib::SimpleQuote> vol(new QuantLib::SimpleQuote( volatility ));
    QuantLib::ext::shared_ptr<QuantLib::BlackVolTermStructure> volTS = flatVol(today, vol, dc);
    QuantLib::ext::shared_ptr<QuantLib::SimpleQuote> qRate(new QuantLib::SimpleQuote( dividendYield ));
    QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure> qTS = flatRate(today, qRate, dc);
    QuantLib::ext::shared_ptr<QuantLib::SimpleQuote> rRate(new QuantLib::SimpleQuote( riskFreeRate ));
    QuantLib::ext::shared_ptr<QuantLib::YieldTermStructure> rTS = flatRate(today, rRate, dc);

    // withDividends = discreteDividends.isNotNull() && discreteDividendsTimeUntil.isNotNull();

#ifdef QL_HIGH_RESOLUTION_DATE
    QuantLib::Date exDate(today.dateTime() + length);
#else
    QuantLib::Date exDate = today + length;
#endif

    QuantLib::ext::shared_ptr<QuantLib::Exercise> exercise(new QuantLib::EuropeanExercise(exDate));
    QuantLib::ext::shared_ptr<QuantLib::StrikedTypePayoff> payoff(new QuantLib::PlainVanillaPayoff(optionType, strike));

    /*if (withDividends) {
        Rcpp::NumericVector divvalues(discreteDividends), divtimes(discreteDividendsTimeUntil);
        int n = divvalues.size();
        std::vector<QuantLib::Date> discDivDates(n);
        std::vector<double> discDividends(n);
        for (int i = 0; i < n; i++) {
#ifdef QL_HIGH_RESOLUTION_DATE
            boost::posix_time::time_duration discreteDividendLength = boost::posix_time::minutes(boost::uint64_t(divtimes[i] * 360 * 24 * 60));
            discDivDates[i] = QuantLib::Date(today.dateTime() + discreteDividendLength);
#else
            discDivDates[i] = today + int(divtimes[i] * 360 + 0.5);
#endif
            discDividends[i] = divvalues[i];
        }*/


            //QuantLib::ext::shared_ptr<QuantLib::BlackScholesMertonProcess>
            QuantLib::ext::shared_ptr<QuantLib::HestonProcess>
            stochProcess(new QuantLib::HestonProcess(QuantLib::Handle<QuantLib::YieldTermStructure>(rTS), //risk free rate
                                                     QuantLib::Handle<QuantLib::YieldTermStructure>(qTS), //dividend
                                                     QuantLib::Handle<QuantLib::Quote>(spot), //s0
                                                     v0, //v0
                                                     kappa, //kappa
                                                     theta, //theta
                                                     sigma, //sigma
                                                     rho)); //rho

        QuantLib::ext::shared_ptr<QuantLib::HestonModel> model(QuantLib::ext::make_shared<QuantLib::HestonModel>(stochProcess));
        //QuantLib::ext::shared_ptr<QuantLib::AnalyticHestonEngine> engine(new QuantLib::HestonModel(stochProcess));
        QuantLib::ext::shared_ptr<QuantLib::AnalyticHestonEngine> lobattoEngine(QuantLib::ext::make_shared<QuantLib::AnalyticHestonEngine>(model, 1e-10,
                                                       1000000));

       // QuantLib::DividendVanillaOption option(payoff, exercise, discDivDates, discDividends);
        QuantLib::VanillaOption option(payoff, exercise);
        option.setPricingEngine(lobattoEngine);

        return Rcpp::List::create(Rcpp::Named("value") = option.NPV());
                                 // Rcpp::Named("delta") = option.delta(),
                                 // Rcpp::Named("gamma") = option.gamma(),
                                 // Rcpp::Named("vega") = option.vega(),
                                 // Rcpp::Named("theta") = option.theta(),
                                  //Rcpp::Named("rho") = option.rho(),
                                 // Rcpp::Named("divRho") = R_NaReal);
    // }
    // else {
    //
    //     QuantLib::ext::shared_ptr<QuantLib::VanillaOption> option = makeOption(payoff, exercise, spot, qTS, rTS, volTS);
    //
    //     return Rcpp::List::create(Rcpp::Named("value") = option->NPV(),
    //                               Rcpp::Named("delta") = option->delta(),
    //                               Rcpp::Named("gamma") = option->gamma(),
    //                               Rcpp::Named("vega") = option->vega(),
    //                               Rcpp::Named("theta") = option->theta(),
    //                               Rcpp::Named("rho") = option->rho(),
    //                               Rcpp::Named("divRho") = option->dividendRho());
    // }

}
