function V=Version
V.Me='v3.2.3';
V.MatlabExtension='14.0.0';
V.MATLAB='R2022b';
persistent NewVersion
try
	if isempty(NewVersion)
		NewVersion=TextAnalytics.CheckUpdateFromGitHub('https://github.com/Silver-Fang/Image5D/releases','Image5D',V.Me);
	end
catch ME
	if ME.identifier~="MATLAB:undefinedVarOrClass"
		ME.rethrow;
	end
end