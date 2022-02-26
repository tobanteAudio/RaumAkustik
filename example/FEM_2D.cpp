// https://podgorskiy.com/spblog/304/writing-a-fem-solver-in-less-the-180-lines-of-code

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Element
{
    void CalculateStiffnessMatrix(const Eigen::Matrix3d& D, std::vector<Eigen::Triplet<double> >& triplets);

    Eigen::Matrix<double, 3, 6> B;
    int nodesIds[3];
};

struct Constraint
{
    enum Type
    {
        UX  = 1 << 0,
        UY  = 1 << 1,
        UXY = UX | UY
    };
    int node;
    Type type;
};

Eigen::VectorXd nodesX;
Eigen::VectorXd nodesY;

void Element::CalculateStiffnessMatrix(const Eigen::Matrix3d& D, std::vector<Eigen::Triplet<double> >& triplets)
{
    Eigen::Vector3d x, y;
    x << nodesX[nodesIds[0]], nodesX[nodesIds[1]], nodesX[nodesIds[2]];
    y << nodesY[nodesIds[0]], nodesY[nodesIds[1]], nodesY[nodesIds[2]];

    Eigen::Matrix3d C;
    C << Eigen::Vector3d(1.0, 1.0, 1.0), x, y;

    Eigen::Matrix3d IC = C.inverse();

    for (int i = 0; i < 3; i++)
    {
        B(0, 2 * i + 0) = IC(1, i);
        B(0, 2 * i + 1) = 0.0;
        B(1, 2 * i + 0) = 0.0;
        B(1, 2 * i + 1) = IC(2, i);
        B(2, 2 * i + 0) = IC(2, i);
        B(2, 2 * i + 1) = IC(1, i);
    }
    Eigen::Matrix<double, 6, 6> K = B.transpose() * D * B * C.determinant() / 2.0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Eigen::Triplet<double> trplt11(2 * nodesIds[i] + 0, 2 * nodesIds[j] + 0, K(2 * i + 0, 2 * j + 0));
            Eigen::Triplet<double> trplt12(2 * nodesIds[i] + 0, 2 * nodesIds[j] + 1, K(2 * i + 0, 2 * j + 1));
            Eigen::Triplet<double> trplt21(2 * nodesIds[i] + 1, 2 * nodesIds[j] + 0, K(2 * i + 1, 2 * j + 0));
            Eigen::Triplet<double> trplt22(2 * nodesIds[i] + 1, 2 * nodesIds[j] + 1, K(2 * i + 1, 2 * j + 1));

            triplets.push_back(trplt11);
            triplets.push_back(trplt12);
            triplets.push_back(trplt21);
            triplets.push_back(trplt22);
        }
    }
}

void SetConstraints(Eigen::SparseMatrix<double>::InnerIterator& it, int index)
{
    if (it.row() == index || it.col() == index) { it.valueRef() = it.row() == it.col() ? 1.0 : 0.0; }
}

void ApplyConstraints(Eigen::SparseMatrix<double>& K, const std::vector<Constraint>& constraints)
{
    std::vector<int> indicesToConstraint;

    for (auto const& c : constraints)
    {
        if (c.type & Constraint::UX) { indicesToConstraint.push_back(2 * c.node + 0); }
        if (c.type & Constraint::UY) { indicesToConstraint.push_back(2 * c.node + 1); }
    }

    for (int k = 0; k < K.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(K, k); it; ++it)
        {
            for (auto const idx : indicesToConstraint) { SetConstraints(it, idx); }
        }
    }
}

auto main(int argc, char* argv[]) -> int
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <input file> <output file>\n";
        return 1;
    }

    std::vector<Element> elements;
    std::vector<Constraint> constraints;

    std::istringstream infile(R"(
0.3 2000
4
0.0 0.0
1.0 0.0
0.0 1.0
1.0 1.0
2
0 1 2
1 3 2
2
0 3
1 2
2
2 0.0 1.0
3 0.0 1.0
)");

    std::ofstream outfile(argv[1]);

    double poissonRatio, youngModulus;
    infile >> poissonRatio >> youngModulus;

    Eigen::Matrix3d D;
    D << 1.0, poissonRatio, 0.0, poissonRatio, 1.0, 0.0, 0.0, 0.0, (1.0 - poissonRatio) / 2.0;

    D *= youngModulus / (1.0 - pow(poissonRatio, 2.0));

    int nodesCount;
    infile >> nodesCount;
    nodesX.resize(nodesCount);
    nodesY.resize(nodesCount);

    for (int i = 0; i < nodesCount; ++i) { infile >> nodesX[i] >> nodesY[i]; }

    int elementCount;
    infile >> elementCount;

    for (int i = 0; i < elementCount; ++i)
    {
        Element element;
        infile >> element.nodesIds[0] >> element.nodesIds[1] >> element.nodesIds[2];
        elements.push_back(element);
    }

    int constraintCount;
    infile >> constraintCount;

    for (int i = 0; i < constraintCount; ++i)
    {
        Constraint constraint;
        int type;
        infile >> constraint.node >> type;
        constraint.type = static_cast<Constraint::Type>(type);
        constraints.push_back(constraint);
    }

    Eigen::VectorXd loads;
    loads.resize(2 * nodesCount);
    loads.setZero();

    int loadsCount;
    infile >> loadsCount;

    for (int i = 0; i < loadsCount; ++i)
    {
        int node;
        double x, y;
        infile >> node >> x >> y;
        loads[2 * node + 0] = x;
        loads[2 * node + 1] = y;
    }

    std::vector<Eigen::Triplet<double> > triplets;
    for (std::vector<Element>::iterator it = elements.begin(); it != elements.end(); ++it)
    {
        it->CalculateStiffnessMatrix(D, triplets);
    }

    Eigen::SparseMatrix<double> globalK(2 * nodesCount, 2 * nodesCount);
    globalK.setFromTriplets(triplets.begin(), triplets.end());

    ApplyConstraints(globalK, constraints);

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solver(globalK);

    Eigen::VectorXd displacements = solver.solve(loads);

    outfile << displacements << std::endl;

    for (std::vector<Element>::iterator it = elements.begin(); it != elements.end(); ++it)
    {
        Eigen::Matrix<double, 6, 1> delta;
        delta << displacements.segment<2>(2 * it->nodesIds[0]), displacements.segment<2>(2 * it->nodesIds[1]),
            displacements.segment<2>(2 * it->nodesIds[2]);

        Eigen::Vector3d sigma = D * it->B * delta;
        double sigma_mises
            = sqrt(sigma[0] * sigma[0] - sigma[0] * sigma[1] + sigma[1] * sigma[1] + 3.0 * sigma[2] * sigma[2]);

        outfile << sigma_mises << std::endl;
    }

    return EXIT_SUCCESS;
}
