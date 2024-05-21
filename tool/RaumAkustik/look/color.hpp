#pragma once

#include <juce_graphics/juce_graphics.h>

namespace ra {

struct ColourRGB
{
    float r;
    float g;
    float b;

    [[nodiscard]] operator juce::Colour() const noexcept  // NOLINT
    {
        return juce::Colour::fromFloatRGBA(r, g, b, 1.0F);
    }
};

static constexpr auto CividisColorMap = std::array{
    ColourRGB{0.0000F, 0.1262F, 0.3015F},
    ColourRGB{0.0000F, 0.1292F, 0.3077F},
    ColourRGB{0.0000F, 0.1321F, 0.3142F},
    ColourRGB{0.0000F, 0.1350F, 0.3205F},
    ColourRGB{0.0000F, 0.1379F, 0.3269F},
    ColourRGB{0.0000F, 0.1408F, 0.3334F},
    ColourRGB{0.0000F, 0.1437F, 0.3400F},
    ColourRGB{0.0000F, 0.1465F, 0.3467F},
    ColourRGB{0.0000F, 0.1492F, 0.3537F},
    ColourRGB{0.0000F, 0.1519F, 0.3606F},
    ColourRGB{0.0000F, 0.1546F, 0.3676F},
    ColourRGB{0.0000F, 0.1574F, 0.3746F},
    ColourRGB{0.0000F, 0.1601F, 0.3817F},
    ColourRGB{0.0000F, 0.1629F, 0.3888F},
    ColourRGB{0.0000F, 0.1657F, 0.3960F},
    ColourRGB{0.0000F, 0.1685F, 0.4031F},
    ColourRGB{0.0000F, 0.1714F, 0.4102F},
    ColourRGB{0.0000F, 0.1743F, 0.4172F},
    ColourRGB{0.0000F, 0.1773F, 0.4241F},
    ColourRGB{0.0000F, 0.1798F, 0.4307F},
    ColourRGB{0.0000F, 0.1817F, 0.4347F},
    ColourRGB{0.0000F, 0.1834F, 0.4363F},
    ColourRGB{0.0000F, 0.1852F, 0.4368F},
    ColourRGB{0.0000F, 0.1872F, 0.4368F},
    ColourRGB{0.0000F, 0.1901F, 0.4365F},
    ColourRGB{0.0000F, 0.1930F, 0.4361F},
    ColourRGB{0.0000F, 0.1958F, 0.4356F},
    ColourRGB{0.0000F, 0.1987F, 0.4349F},
    ColourRGB{0.0000F, 0.2015F, 0.4343F},
    ColourRGB{0.0000F, 0.2044F, 0.4336F},
    ColourRGB{0.0000F, 0.2073F, 0.4329F},
    ColourRGB{0.0055F, 0.2101F, 0.4322F},
    ColourRGB{0.0236F, 0.2130F, 0.4314F},
    ColourRGB{0.0416F, 0.2158F, 0.4308F},
    ColourRGB{0.0576F, 0.2187F, 0.4301F},
    ColourRGB{0.0710F, 0.2215F, 0.4293F},
    ColourRGB{0.0827F, 0.2244F, 0.4287F},
    ColourRGB{0.0932F, 0.2272F, 0.4280F},
    ColourRGB{0.1030F, 0.2300F, 0.4274F},
    ColourRGB{0.1120F, 0.2329F, 0.4268F},
    ColourRGB{0.1204F, 0.2357F, 0.4262F},
    ColourRGB{0.1283F, 0.2385F, 0.4256F},
    ColourRGB{0.1359F, 0.2414F, 0.4251F},
    ColourRGB{0.1431F, 0.2442F, 0.4245F},
    ColourRGB{0.1500F, 0.2470F, 0.4241F},
    ColourRGB{0.1566F, 0.2498F, 0.4236F},
    ColourRGB{0.1630F, 0.2526F, 0.4232F},
    ColourRGB{0.1692F, 0.2555F, 0.4228F},
    ColourRGB{0.1752F, 0.2583F, 0.4224F},
    ColourRGB{0.1811F, 0.2611F, 0.4220F},
    ColourRGB{0.1868F, 0.2639F, 0.4217F},
    ColourRGB{0.1923F, 0.2667F, 0.4214F},
    ColourRGB{0.1977F, 0.2695F, 0.4212F},
    ColourRGB{0.2030F, 0.2723F, 0.4209F},
    ColourRGB{0.2082F, 0.2751F, 0.4207F},
    ColourRGB{0.2133F, 0.2780F, 0.4205F},
    ColourRGB{0.2183F, 0.2808F, 0.4204F},
    ColourRGB{0.2232F, 0.2836F, 0.4203F},
    ColourRGB{0.2281F, 0.2864F, 0.4202F},
    ColourRGB{0.2328F, 0.2892F, 0.4201F},
    ColourRGB{0.2375F, 0.2920F, 0.4200F},
    ColourRGB{0.2421F, 0.2948F, 0.4200F},
    ColourRGB{0.2466F, 0.2976F, 0.4200F},
    ColourRGB{0.2511F, 0.3004F, 0.4201F},
    ColourRGB{0.2556F, 0.3032F, 0.4201F},
    ColourRGB{0.2599F, 0.3060F, 0.4202F},
    ColourRGB{0.2643F, 0.3088F, 0.4203F},
    ColourRGB{0.2686F, 0.3116F, 0.4205F},
    ColourRGB{0.2728F, 0.3144F, 0.4206F},
    ColourRGB{0.2770F, 0.3172F, 0.4208F},
    ColourRGB{0.2811F, 0.3200F, 0.4210F},
    ColourRGB{0.2853F, 0.3228F, 0.4212F},
    ColourRGB{0.2894F, 0.3256F, 0.4215F},
    ColourRGB{0.2934F, 0.3284F, 0.4218F},
    ColourRGB{0.2974F, 0.3312F, 0.4221F},
    ColourRGB{0.3014F, 0.3340F, 0.4224F},
    ColourRGB{0.3054F, 0.3368F, 0.4227F},
    ColourRGB{0.3093F, 0.3396F, 0.4231F},
    ColourRGB{0.3132F, 0.3424F, 0.4236F},
    ColourRGB{0.3170F, 0.3453F, 0.4240F},
    ColourRGB{0.3209F, 0.3481F, 0.4244F},
    ColourRGB{0.3247F, 0.3509F, 0.4249F},
    ColourRGB{0.3285F, 0.3537F, 0.4254F},
    ColourRGB{0.3323F, 0.3565F, 0.4259F},
    ColourRGB{0.3361F, 0.3593F, 0.4264F},
    ColourRGB{0.3398F, 0.3622F, 0.4270F},
    ColourRGB{0.3435F, 0.3650F, 0.4276F},
    ColourRGB{0.3472F, 0.3678F, 0.4282F},
    ColourRGB{0.3509F, 0.3706F, 0.4288F},
    ColourRGB{0.3546F, 0.3734F, 0.4294F},
    ColourRGB{0.3582F, 0.3763F, 0.4302F},
    ColourRGB{0.3619F, 0.3791F, 0.4308F},
    ColourRGB{0.3655F, 0.3819F, 0.4316F},
    ColourRGB{0.3691F, 0.3848F, 0.4322F},
    ColourRGB{0.3727F, 0.3876F, 0.4331F},
    ColourRGB{0.3763F, 0.3904F, 0.4338F},
    ColourRGB{0.3798F, 0.3933F, 0.4346F},
    ColourRGB{0.3834F, 0.3961F, 0.4355F},
    ColourRGB{0.3869F, 0.3990F, 0.4364F},
    ColourRGB{0.3905F, 0.4018F, 0.4372F},
    ColourRGB{0.3940F, 0.4047F, 0.4381F},
    ColourRGB{0.3975F, 0.4075F, 0.4390F},
    ColourRGB{0.4010F, 0.4104F, 0.4400F},
    ColourRGB{0.4045F, 0.4132F, 0.4409F},
    ColourRGB{0.4080F, 0.4161F, 0.4419F},
    ColourRGB{0.4114F, 0.4189F, 0.4430F},
    ColourRGB{0.4149F, 0.4218F, 0.4440F},
    ColourRGB{0.4183F, 0.4247F, 0.4450F},
    ColourRGB{0.4218F, 0.4275F, 0.4462F},
    ColourRGB{0.4252F, 0.4304F, 0.4473F},
    ColourRGB{0.4286F, 0.4333F, 0.4485F},
    ColourRGB{0.4320F, 0.4362F, 0.4496F},
    ColourRGB{0.4354F, 0.4390F, 0.4508F},
    ColourRGB{0.4388F, 0.4419F, 0.4521F},
    ColourRGB{0.4422F, 0.4448F, 0.4534F},
    ColourRGB{0.4456F, 0.4477F, 0.4547F},
    ColourRGB{0.4489F, 0.4506F, 0.4561F},
    ColourRGB{0.4523F, 0.4535F, 0.4575F},
    ColourRGB{0.4556F, 0.4564F, 0.4589F},
    ColourRGB{0.4589F, 0.4593F, 0.4604F},
    ColourRGB{0.4622F, 0.4622F, 0.4620F},
    ColourRGB{0.4656F, 0.4651F, 0.4635F},
    ColourRGB{0.4689F, 0.4680F, 0.4650F},
    ColourRGB{0.4722F, 0.4709F, 0.4665F},
    ColourRGB{0.4756F, 0.4738F, 0.4679F},
    ColourRGB{0.4790F, 0.4767F, 0.4691F},
    ColourRGB{0.4825F, 0.4797F, 0.4701F},
    ColourRGB{0.4861F, 0.4826F, 0.4707F},
    ColourRGB{0.4897F, 0.4856F, 0.4714F},
    ColourRGB{0.4934F, 0.4886F, 0.4719F},
    ColourRGB{0.4971F, 0.4915F, 0.4723F},
    ColourRGB{0.5008F, 0.4945F, 0.4727F},
    ColourRGB{0.5045F, 0.4975F, 0.4730F},
    ColourRGB{0.5083F, 0.5005F, 0.4732F},
    ColourRGB{0.5121F, 0.5035F, 0.4734F},
    ColourRGB{0.5158F, 0.5065F, 0.4736F},
    ColourRGB{0.5196F, 0.5095F, 0.4737F},
    ColourRGB{0.5234F, 0.5125F, 0.4738F},
    ColourRGB{0.5272F, 0.5155F, 0.4739F},
    ColourRGB{0.5310F, 0.5186F, 0.4739F},
    ColourRGB{0.5349F, 0.5216F, 0.4738F},
    ColourRGB{0.5387F, 0.5246F, 0.4739F},
    ColourRGB{0.5425F, 0.5277F, 0.4738F},
    ColourRGB{0.5464F, 0.5307F, 0.4736F},
    ColourRGB{0.5502F, 0.5338F, 0.4735F},
    ColourRGB{0.5541F, 0.5368F, 0.4733F},
    ColourRGB{0.5579F, 0.5399F, 0.4732F},
    ColourRGB{0.5618F, 0.5430F, 0.4729F},
    ColourRGB{0.5657F, 0.5461F, 0.4727F},
    ColourRGB{0.5696F, 0.5491F, 0.4723F},
    ColourRGB{0.5735F, 0.5522F, 0.4720F},
    ColourRGB{0.5774F, 0.5553F, 0.4717F},
    ColourRGB{0.5813F, 0.5584F, 0.4714F},
    ColourRGB{0.5852F, 0.5615F, 0.4709F},
    ColourRGB{0.5892F, 0.5646F, 0.4705F},
    ColourRGB{0.5931F, 0.5678F, 0.4701F},
    ColourRGB{0.5970F, 0.5709F, 0.4696F},
    ColourRGB{0.6010F, 0.5740F, 0.4691F},
    ColourRGB{0.6050F, 0.5772F, 0.4685F},
    ColourRGB{0.6089F, 0.5803F, 0.4680F},
    ColourRGB{0.6129F, 0.5835F, 0.4673F},
    ColourRGB{0.6168F, 0.5866F, 0.4668F},
    ColourRGB{0.6208F, 0.5898F, 0.4662F},
    ColourRGB{0.6248F, 0.5929F, 0.4655F},
    ColourRGB{0.6288F, 0.5961F, 0.4649F},
    ColourRGB{0.6328F, 0.5993F, 0.4641F},
    ColourRGB{0.6368F, 0.6025F, 0.4632F},
    ColourRGB{0.6408F, 0.6057F, 0.4625F},
    ColourRGB{0.6449F, 0.6089F, 0.4617F},
    ColourRGB{0.6489F, 0.6121F, 0.4609F},
    ColourRGB{0.6529F, 0.6153F, 0.4600F},
    ColourRGB{0.6570F, 0.6185F, 0.4591F},
    ColourRGB{0.6610F, 0.6217F, 0.4583F},
    ColourRGB{0.6651F, 0.6250F, 0.4573F},
    ColourRGB{0.6691F, 0.6282F, 0.4562F},
    ColourRGB{0.6732F, 0.6315F, 0.4553F},
    ColourRGB{0.6773F, 0.6347F, 0.4543F},
    ColourRGB{0.6813F, 0.6380F, 0.4532F},
    ColourRGB{0.6854F, 0.6412F, 0.4521F},
    ColourRGB{0.6895F, 0.6445F, 0.4511F},
    ColourRGB{0.6936F, 0.6478F, 0.4499F},
    ColourRGB{0.6977F, 0.6511F, 0.4487F},
    ColourRGB{0.7018F, 0.6544F, 0.4475F},
    ColourRGB{0.7060F, 0.6577F, 0.4463F},
    ColourRGB{0.7101F, 0.6610F, 0.4450F},
    ColourRGB{0.7142F, 0.6643F, 0.4437F},
    ColourRGB{0.7184F, 0.6676F, 0.4424F},
    ColourRGB{0.7225F, 0.6710F, 0.4409F},
    ColourRGB{0.7267F, 0.6743F, 0.4396F},
    ColourRGB{0.7308F, 0.6776F, 0.4382F},
    ColourRGB{0.7350F, 0.6810F, 0.4368F},
    ColourRGB{0.7392F, 0.6844F, 0.4352F},
    ColourRGB{0.7434F, 0.6877F, 0.4338F},
    ColourRGB{0.7476F, 0.6911F, 0.4322F},
    ColourRGB{0.7518F, 0.6945F, 0.4307F},
    ColourRGB{0.7560F, 0.6979F, 0.4290F},
    ColourRGB{0.7602F, 0.7013F, 0.4273F},
    ColourRGB{0.7644F, 0.7047F, 0.4258F},
    ColourRGB{0.7686F, 0.7081F, 0.4241F},
    ColourRGB{0.7729F, 0.7115F, 0.4223F},
    ColourRGB{0.7771F, 0.7150F, 0.4205F},
    ColourRGB{0.7814F, 0.7184F, 0.4188F},
    ColourRGB{0.7856F, 0.7218F, 0.4168F},
    ColourRGB{0.7899F, 0.7253F, 0.4150F},
    ColourRGB{0.7942F, 0.7288F, 0.4129F},
    ColourRGB{0.7985F, 0.7322F, 0.4111F},
    ColourRGB{0.8027F, 0.7357F, 0.4090F},
    ColourRGB{0.8070F, 0.7392F, 0.4070F},
    ColourRGB{0.8114F, 0.7427F, 0.4049F},
    ColourRGB{0.8157F, 0.7462F, 0.4028F},
    ColourRGB{0.8200F, 0.7497F, 0.4007F},
    ColourRGB{0.8243F, 0.7532F, 0.3984F},
    ColourRGB{0.8287F, 0.7568F, 0.3961F},
    ColourRGB{0.8330F, 0.7603F, 0.3938F},
    ColourRGB{0.8374F, 0.7639F, 0.3915F},
    ColourRGB{0.8417F, 0.7674F, 0.3892F},
    ColourRGB{0.8461F, 0.7710F, 0.3869F},
    ColourRGB{0.8505F, 0.7745F, 0.3843F},
    ColourRGB{0.8548F, 0.7781F, 0.3818F},
    ColourRGB{0.8592F, 0.7817F, 0.3793F},
    ColourRGB{0.8636F, 0.7853F, 0.3766F},
    ColourRGB{0.8681F, 0.7889F, 0.3739F},
    ColourRGB{0.8725F, 0.7926F, 0.3712F},
    ColourRGB{0.8769F, 0.7962F, 0.3684F},
    ColourRGB{0.8813F, 0.7998F, 0.3657F},
    ColourRGB{0.8858F, 0.8035F, 0.3627F},
    ColourRGB{0.8902F, 0.8071F, 0.3599F},
    ColourRGB{0.8947F, 0.8108F, 0.3569F},
    ColourRGB{0.8992F, 0.8145F, 0.3538F},
    ColourRGB{0.9037F, 0.8182F, 0.3507F},
    ColourRGB{0.9082F, 0.8219F, 0.3474F},
    ColourRGB{0.9127F, 0.8256F, 0.3442F},
    ColourRGB{0.9172F, 0.8293F, 0.3409F},
    ColourRGB{0.9217F, 0.8330F, 0.3374F},
    ColourRGB{0.9262F, 0.8367F, 0.3340F},
    ColourRGB{0.9308F, 0.8405F, 0.3306F},
    ColourRGB{0.9353F, 0.8442F, 0.3268F},
    ColourRGB{0.9399F, 0.8480F, 0.3232F},
    ColourRGB{0.9444F, 0.8518F, 0.3195F},
    ColourRGB{0.9490F, 0.8556F, 0.3155F},
    ColourRGB{0.9536F, 0.8593F, 0.3116F},
    ColourRGB{0.9582F, 0.8632F, 0.3076F},
    ColourRGB{0.9628F, 0.8670F, 0.3034F},
    ColourRGB{0.9674F, 0.8708F, 0.2990F},
    ColourRGB{0.9721F, 0.8746F, 0.2947F},
    ColourRGB{0.9767F, 0.8785F, 0.2901F},
    ColourRGB{0.9814F, 0.8823F, 0.2856F},
    ColourRGB{0.9860F, 0.8862F, 0.2807F},
    ColourRGB{0.9907F, 0.8901F, 0.2759F},
    ColourRGB{0.9954F, 0.8940F, 0.2708F},
    ColourRGB{1.0000F, 0.8979F, 0.2655F},
    ColourRGB{1.0000F, 0.9018F, 0.2600F},
    ColourRGB{1.0000F, 0.9057F, 0.2593F},
    ColourRGB{1.0000F, 0.9094F, 0.2634F},
    ColourRGB{1.0000F, 0.9131F, 0.2680F},
    ColourRGB{1.0000F, 0.9169F, 0.2731F},
};

}  // namespace ra