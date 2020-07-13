#Brief configuration file for creating categories in the models
print("Loading Categories...")

tt_0jet_category = 'tt_0jet'
tt_boosted_1J_category = 'tt_boosted_onejet'
tt_boosted_GE2J_category = 'tt_boosted_multijet'
tt_vbf_high_category = 'tt_vbf_highHpT'
tt_vbf_low_category = 'tt_vbf_lowHpT'

mt_0jet_category = 'mt_0jet'
#mt_0jet_low_category = 'mt_0jet_PTH_0_10'
#mt_0jet_high_category = 'mt_0jet_PTH_GE10'
mt_boosted_1J_category = 'mt_boosted_1J'
mt_boosted_GE2J_category = 'mt_boosted_GE2J'
mt_vbf_low_category = 'mt_vbf_PTH_0_200'
mt_vbf_high_category = 'mt_vbf_PTH_GE_200'

et_0jet_category = 'et_0jet'
#et_0jet_low_category = 'et_0jetlow'
#et_0jet_high_category = 'et_0jethigh'
et_boosted_1J_category = 'et_boosted1'
et_boosted_GE2J_category = 'et_boosted2'
et_vbf_low_category = 'et_vbflow'
et_vbf_high_category = 'et_vbfhigh'

#em_0jet_low_category = 'em_0jetlow'
#em_0jet_high_category = 'em_0jethigh'
em_0jet_category = 'em_0jet'
em_boosted_1J_category = 'em_boosted1'
em_boosted_GE2J_category = 'em_boosted2'
em_vbf_low_category = 'em_vbflow'
em_vbf_high_category = 'em_vbfhigh'

tt_boosted_categories = []
tt_boosted_categories.append(tt_boosted_1J_category)
tt_boosted_categories.append(tt_boosted_GE2J_category)
tt_vbf_categories = []
tt_vbf_categories.append(tt_vbf_low_category)
tt_vbf_categories.append(tt_vbf_high_category)
tt_0jet_categories = []
tt_0jet_categories.append(tt_0jet_category)

tt_Categories = []
tt_Categories = tt_0jet_categories + tt_boosted_categories + tt_vbf_categories

mt_0jet_categories = []
mt_0jet_categories.append(mt_0jet_category)
#mt_0jet_categories.append(mt_0jet_low_category)
#mt_0jet_categories.append(mt_0jet_high_category)
mt_boosted_categories = []
mt_boosted_categories.append(mt_boosted_1J_category)
mt_boosted_categories.append(mt_boosted_GE2J_category)
mt_vbf_categories = []
mt_vbf_categories.append(mt_vbf_low_category)
mt_vbf_categories.append(mt_vbf_high_category)

mt_Categories = []
mt_Categories =  mt_boosted_categories + mt_vbf_categories + mt_0jet_categories

et_boosted_categories = []
et_boosted_categories.append(et_boosted_1J_category)
et_boosted_categories.append(et_boosted_GE2J_category)
et_vbf_categories = []
et_vbf_categories.append(et_vbf_low_category)
et_vbf_categories.append(et_vbf_high_category)
et_0jet_categories = []
et_0jet_categories.append(et_0jet_category)
#et_0jet_categories.append(et_0jet_low_category)
#et_0jet_categories.append(et_0jet_high_category)

et_Categories = []
et_Categories = et_0jet_categories + et_boosted_categories + et_vbf_categories

em_boosted_categories = []
em_boosted_categories.append(em_boosted_1J_category)
em_boosted_categories.append(em_boosted_GE2J_category)
em_vbf_categories = []
em_vbf_categories.append(em_vbf_low_category)
em_vbf_categories.append(em_vbf_high_category)
em_0jet_categories = []
em_0jet_categories.append(em_0jet_category)
#em_0jet_categories.append(em_0jet_low_category)
#em_0jet_categories.append(em_0jet_high_category)

em_Categories = []
em_Categories = em_0jet_categories + em_boosted_categories + em_vbf_categories 

Categories = {'tt':tt_Categories,
              'mt':mt_Categories,
              'et':et_Categories,
              'em':em_Categories}

#Let's do bin and rolling setup as well.
standardRecoBinConfiguration = [50,70,90,110,130,150,170,210,250,290]
recoBinConfiguration = {
    'tt':{
        tt_0jet_category:standardRecoBinConfiguration,
        tt_boosted_1J_category:standardRecoBinConfiguration,
        tt_boosted_GE2J_category:standardRecoBinConfiguration,
        tt_vbf_high_category:standardRecoBinConfiguration,
        tt_vbf_low_category:standardRecoBinConfiguration,
    },
    'mt':{
        mt_0jet_category:standardRecoBinConfiguration,
        mt_boosted_1J_category:standardRecoBinConfiguration,
        mt_boosted_GE2J_category:standardRecoBinConfiguration,
        mt_vbf_low_category:standardRecoBinConfiguration,
        mt_vbf_high_category:standardRecoBinConfiguration,
    },
    'et':{
        et_0jet_category:standardRecoBinConfiguration,
        et_boosted_1J_category:standardRecoBinConfiguration,
        et_boosted_GE2J_category:standardRecoBinConfiguration,
        et_vbf_low_category:standardRecoBinConfiguration,
        et_vbf_high_category:standardRecoBinConfiguration,
    },
    'em':{
	em_0jet_category:standardRecoBinConfiguration,
        #em_0jet_low_category:standardRecoBinConfiguration,
        #em_0jet_high_category:standardRecoBinConfiguration,
        em_boosted_1J_category:standardRecoBinConfiguration,
        em_boosted_GE2J_category:standardRecoBinConfiguration,
        em_vbf_low_category:standardRecoBinConfiguration,
        em_vbf_high_category:standardRecoBinConfiguration,
    },
}

rollingBinConfiguration = {
    'tt':{
        tt_0jet_category:[],
        tt_boosted_1J_category:[],
        tt_boosted_GE2J_category:[],
        tt_vbf_high_category:[],
        tt_vbf_low_category:[],
    },
    'mt':{
        mt_0jet_category:[30,40,50,10000],
        mt_boosted_1J_category:[0,60,120,200,250,10000],
        mt_boosted_GE2J_category:[0,60,120,200,250,10000],
        mt_vbf_low_category:[350,700,1000,1500,1800,10000],
        mt_vbf_high_category:[350,700,1200,10000],
    },
    'et':{
        et_0jet_category:[30,40,50,9000],
        et_boosted_1J_category:[0,60,120,200,250,9000],
        et_boosted_GE2J_category:[0,60,120,200,250,9000],
        et_vbf_low_category:[350,700,1000,1500,1800,9000],
        et_vbf_high_category:[350,700,1200,9000],
    },
    'em':{
        em_0jet_category:[0,9000],
        #em_0jet_low_category:[30,40,50,9000],
        #em_0jet_high_category:[30,40,50,9000],
        em_boosted_1J_category:[0,60,120,200,250,9000],
        em_boosted_GE2J_category:[0,60,120,200,250,9000],
        em_vbf_low_category:[350,700,1000,1500,1800,9000],
        em_vbf_high_category:[350,700,1000,1500,1800,9000],
    },
}
