function V = Version(AlwaysCheck)
arguments
	AlwaysCheck=false;
end
persistent pVersion
if isempty(pVersion)||AlwaysCheck
	V='3.3.2';
	if~isempty(metafunction('TextAnalytics.CheckUpdateFromGitHub'))
		TextAnalytics.CheckUpdateFromGitHub('https://github.com/Ebola-Chan-bot/Image5D/releases','Image5D',['v',V]);
	end
	pVersion.Me=matlab.mpm.Version(V);
	pVersion.MATLAB='R2024b';
	pVersion.Dependencies=array2table(["MATLAB"	"20.2.4"	"https://github.com/Ebola-Chan-bot/MATLAB-Extension/releases"],VariableNames=["Namespace","Version","URL"]);
	pVersion.Dependencies.Version=matlab.mpm.Version(pVersion.Dependencies.Version);
	for D=1:height(pVersion.Dependencies)
		Namespace=pVersion.Dependencies.Namespace(D);
		MpmVersion=Namespace+".Version";
		EarliestVersion=pVersion.Dependencies.Version(D);
		URL=pVersion.Dependencies.URL(D);
		if isempty(metafunction(MpmVersion))
			warning('Image5D:Exception:Lack_dependence','Image5D:Exception:Lack_dependence：<a href="%s">%s v%s</a>',URL,Namespace,EarliestVersion);
		else
			MpmVersion=feval(MpmVersion).Me;
			for F=["Major","Minor","Patch"]
				if EarliestVersion.(F)<MpmVersion.(F)
					break;
				elseif EarliestVersion.(F)>MpmVersion.(F)
					warning('Image5D:Exception:Dependence_version_too_low','Image5D:Exception:Dependence_version_too_low：<a href="%s">%s v%s</a>',URL,Namespace,EarliestVersion);
					break;
				end
			end
		end
	end
end
V=pVersion;