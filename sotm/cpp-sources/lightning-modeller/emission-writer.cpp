#include "emission-writer.hpp"

EmissionWriter::EmissionWriter(cic::Parameters& p)
{
    p.addGroup(m_pg);
 /*   m_receiver_position[0] = pg.get<double>("receiver-x");
    m_receiver_position[1] = pg.get<double>("receiver-y");
    m_receiver_position[2] = pg.get<double>("receiver-z");*/
}
