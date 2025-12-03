#include "bdd.h"
#include "config.h"
#include "ui.h"
#include "zheg.h"

int main() {
    auto vars = config::variables();
    auto truth = config::truthVector();

    ZhegalkinPolynomial polynomial(truth);
    BDDGraph bdd(truth, vars);
    ConsoleUi ui(vars, truth, polynomial, bdd);
    ui.run();

    return 0;
}
