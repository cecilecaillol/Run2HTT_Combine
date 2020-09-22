from HiggsAnalysis.CombinedLimit.PhysicsModel import *
import re

class FiducialNJETS(PhysicsModel):
    def doParametersOfInterest(self):
        """Create POI and other parameters, and define the POI set."""
        self.modelBuilder.doVar("mu_fid[1.0,-15.0,15.0]");
        self.modelBuilder.doVar("rho_0[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_1[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_2[1.0,-25.0,25.0]");
        self.modelBuilder.doVar("rho_3[1.0,-25.0,25.0]");
        pois = 'mu_fid,rho_0,rho_1,rho_2,rho_3'
        self.modelBuilder.doSet("POI",pois)
        if self.options.mass != 0:
            if self.modelBuilder.out.var("MH"):
              self.modelBuilder.out.var("MH").removeRange()
              self.modelBuilder.out.var("MH").setVal(self.options.mass)
            else:
              self.modelBuilder.doVar("MH[%g]" % self.options.mass);
	self.modelBuilder.factory_('expr::scale_0("@0*@1",mu_fid,rho_0)')
        self.modelBuilder.factory_('expr::scale_1("@0*@1",mu_fid,rho_1)')
        self.modelBuilder.factory_('expr::scale_2("@0*@1",mu_fid,rho_2)')
        self.modelBuilder.factory_('expr::scale_3("@0*@1",mu_fid,rho_3)')
        self.modelBuilder.factory_('expr::scale_GE4("@0*(485.9-@1*250.2-@2*139.1-@3*69.8-@4*19.3)/7.425",mu_fid,rho_0,rho_1,rho_2,rho_3)')
    def getYieldScale(self,bin,process):
        "Return the name of a RooAbsReal to scale this yield by or the two special values 1 and 0 (don't scale, and set to zero)"
	if re.search("NJETS_0",process):
	   return "scale_0"
        if re.search("NJETS_1",process):
           return "scale_1"
        if re.search("NJETS_2",process):
           return "scale_2"
        if re.search("NJETS_3",process):
           return "scale_3"
        if re.search("NJETS_GE4",process):
           return "scale_GE4"
        return 1

fiducialNJETS = FiducialNJETS()
