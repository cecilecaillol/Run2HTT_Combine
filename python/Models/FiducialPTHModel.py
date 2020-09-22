from HiggsAnalysis.CombinedLimit.PhysicsModel import *
import re

class FiducialPTH(PhysicsModel):
    def doParametersOfInterest(self):
        """Create POI and other parameters, and define the POI set."""
        self.modelBuilder.doVar("mu_fid[1.0,-15.0,15.0]");
        self.modelBuilder.doVar("rho_0_45[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_45_80[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_80_120[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_120_200[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_200_350[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_350_450[1.0,-25.0,25.0]");
        pois = 'mu_fid,rho_0_45,rho_45_80,rho_80_120,rho_120_200,rho_200_350,rho_350_450'
        self.modelBuilder.doSet("POI",pois)
        if self.options.mass != 0:
            if self.modelBuilder.out.var("MH"):
              self.modelBuilder.out.var("MH").removeRange()
              self.modelBuilder.out.var("MH").setVal(self.options.mass)
            else:
              self.modelBuilder.doVar("MH[%g]" % self.options.mass);
	self.modelBuilder.factory_('expr::scale_0_45("@0*@1",mu_fid,rho_0_45)')
        self.modelBuilder.factory_('expr::scale_45_80("@0*@1",mu_fid,rho_45_80)')
        self.modelBuilder.factory_('expr::scale_80_120("@0*@1",mu_fid,rho_80_120)')
        self.modelBuilder.factory_('expr::scale_120_200("@0*@1",mu_fid,rho_120_200)')
        self.modelBuilder.factory_('expr::scale_200_350("@0*@1",mu_fid,rho_200_350)')
        self.modelBuilder.factory_('expr::scale_350_450("@0*@1",mu_fid,rho_350_450)')
        self.modelBuilder.factory_('expr::scale_GT450("@0*(486.1-@1*287.2-@2*85.3-@3*48.0-@4*43.8-@5*18.8-@6*2.26)/0.735",mu_fid,rho_0_45,rho_45_80,rho_80_120,rho_120_200,rho_200_350,rho_350_450)')
    def getYieldScale(self,bin,process):
        "Return the name of a RooAbsReal to scale this yield by or the two special values 1 and 0 (don't scale, and set to zero)"
	if re.search("PTH_0_45",process):
	   return "scale_0_45"
        if re.search("PTH_45_80",process):
           return "scale_45_80"
        if re.search("PTH_80_120",process):
           return "scale_80_120"
        if re.search("PTH_120_200",process):
           return "scale_120_200"
        if re.search("PTH_200_350",process):
           return "scale_200_350"
        if re.search("PTH_350_450",process):
           return "scale_350_450"
        if re.search("PTH_GT450",process):
           return "scale_GT450"
        return 1

fiducialPTH = FiducialPTH()
