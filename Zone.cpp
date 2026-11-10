#include "Zone.h"

Zone::Zone(std::string n, ZoneType t, double infMult, double moveMod)
    : name(n), type(t), infectionMultiplier(infMult), movementModifier(moveMod)
{
    
}