/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bepoisso <bepoisso@student.perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 09:50:11 by bepoisso          #+#    #+#             */
/*   Updated: 2025/05/06 13:56:07 by bepoisso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void	ft_error(char *s, char *arg)
{
	while (*s)
		write(2, s++, 1);
	if (arg)
		while (*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
}

int	ft_cd(char **av, int i)
{
	if (i != 2)
		return (ft_error("error: cd: bad arguments", NULL), 1);
	if (chdir(av[1]) == -1)
		return (ft_error("error: cd: cannot change directory to ", av[1]), 1);
	return (0);
}

void	set_pipe(int has_pipe, int *fd, int end)
{
	if (has_pipe && (dup2(fd[end], end) == -1 || close (fd[0]) || close(fd[1]) == -1))
		ft_error("error: fatal", NULL), exit(1);
}

int	ft_exec(char **av, int i, char **env)
{
	int	has_pipe;
	int	fd[2];
	int	pid;
	int	status;

	has_pipe = av[i] && strcmp(av[i], "|") == 0;
	if (!has_pipe && strcmp(*av, "cd") == 0)
		return (ft_cd(av, i));
	if (has_pipe && pipe(fd) == -1)
		ft_error("error: fatal", NULL), exit(1);
	if ((pid = fork()) == -1)
		ft_error("error: fatal", NULL), exit(1);
	if (!pid)
	{
		av[i] = NULL;
		set_pipe(has_pipe, fd, 1);
		if (strcmp(*av, "cd") == 0)
			exit (ft_cd(av, i));
		execve(*av, av, env);
		ft_error("error: cannot execute ", *av);
		exit (1);
	}
	waitpid(pid, &status, 0);
	set_pipe(has_pipe, fd, 0);
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int main(int ac, char **av, char **env)
{
	int	i = 0;
	int	status = 0;

	(void)ac;
	while (av[i])
	{
		av += i + 1;
		i = 0;
		while (av[i] && strcmp(av[i], "|") != 0 && strcmp(av[i], ";") != 0)
			i++;
		if (i)
			status = ft_exec(av, i, env);
	}
	return (status);
}
