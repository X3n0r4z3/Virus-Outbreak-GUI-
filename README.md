# Virus Outbreak Rescue: Tactical Command Center

A real-time epidemiological containment and strategy simulation built in C++ using wxWidgets. Take command of a global sector map, deploy emergency medical infrastructure, distribute mass vaccines, and monitor outbreak vectors in real time before the pathogen overwhelms the grid.

---

## 🎮 The Game Loop

- **Turn Progression (Simulation Tick):** The simulation advances a turn every 500 milliseconds (2 turns per second) via `sim->runTurn()`. During each tick, the viral outbreak propagates across adjacent grid cells based on infection rates, while health states transition:
  $$\text{Healthy} \rightarrow \text{Infected} \rightarrow \text{Recovered or Deceased}$$
- **Live Telemetry & Monitoring:** The status bar continuously feeds real-time sector population counts for **Healthy**, **Infected**, and **Deceased** personnel.
- **Win/Loss Conditions:**
  - **Victory:** Reduce active infections to zero (`getInfectedCount() == 0`) while preserving survivors to successfully contain the outbreak and trigger victory protocols.
  - **Defeat:** If the infection completely wipes out the population, emergency sirens sound and containment failure protocols execute.

---

## 🕹️ How You Play

As the tactical commander, your objective is to choke off the pathogen's spread across the sector grid:

1. **Construct Medical Infrastructure:**
   - **Regular Hospitals (15-bed capacity):** Deploy localized medical support to treat infected cells.
   - **Super Hospitals (100-bed capacity):** Establish high-value command anchors equipped with automated supply response vector lines that lock onto nearby infection clusters.
2. **Distribute Vaccines:** Deploy mass immunization payloads (`Distribute Vaccines` button) to turn healthy sectors **Electric Blue**, creating a cryogenic containment barrier that stops the virus dead in its tracks.
3. **Toggle Tactical Scanners:** Switch dynamically between the **Standard Tactical Grid View** (featuring coordinate markers like `A0`–`Z`, scanlines, and sector tinting) and the **Thermal Heat Map Scan** to analyze high-density infection heat plumes in real time.
