#include	"ExtResampler.h"

static	void	resample_with_reverse( float *pout, int numsamples, CExtResamplerState &state, CExtResamplerParams const &params )
{
	if( state.Active )
	{
		__int64	d=((__int64(params.StepInt)<<RS_STEP_FRAC_BITS)+params.StepFrac);
		__int64	p=((__int64(state.PosInt)<<RS_STEP_FRAC_BITS)+state.PosFrac);

		if( p+d*numsamples>=0 )
		{
			DSP_Resample( pout, numsamples, state, params  );
		}
		else
		{
			while( state.Active && numsamples>0 )
			{
				int	n;
				
				if( (params.LoopBegin!=-1) && (p>=(__int64(params.LoopBegin)<<RS_STEP_FRAC_BITS)) )
					n=int((p-(__int64(params.LoopBegin)<<RS_STEP_FRAC_BITS))/-d);
				else
					n=int(p/-d);

				if( numsamples<n )
					n=numsamples;

				if( n>0 )
				{
					DSP_Resample( pout, n, state, params  );
					numsamples-=n;
					if( params.LoopBegin!=-1 && (((__int64(state.PosInt)<<RS_STEP_FRAC_BITS)+state.PosFrac)<=(__int64(params.LoopBegin)<<RS_STEP_FRAC_BITS)) )
					{
						state.SetPos(params.numSamples);
					}
				}
				else
				{
					state.Active=false;
				}
			}
		}
	}
}

void	EXTDSP_Resample( float *pout, int numsamples, CExtResamplerState &state, CExtResamplerParams const &params )
{
	if( state.Active )
	{
		if( params.AmpMode==RSA_LINEAR_INTP )
		{
			int	n;

			n=int((params.DestAmp-state.Amp)/params.AmpStep);

			if( numsamples<n )
				n=numsamples;

			if( n>0 )
			{
				resample_with_reverse( pout, n, state, params );
				numsamples-=n;
			}

			if( numsamples>0 )
			{
				state.Amp=params.DestAmp;
				CExtResamplerParams	p=params;
				p.AmpMode=RSA_CONSTANT;
				p.AmpStep=0.0f;
				resample_with_reverse( pout, numsamples, state, p );
			}
		}
		else
		{
			resample_with_reverse( pout, numsamples, state, params );
		}
	}
}

